package com.github.nkzawa.socketio.client;

import com.github.nkzawa.emitter.Emitter;
import com.github.nkzawa.socketio.parser.Packet;
import com.github.nkzawa.socketio.parser.Parser;
import com.github.nkzawa.thread.EventThread;

import javax.net.ssl.SSLContext;
import java.net.URI;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.*;
import java.util.logging.Level;
import java.util.logging.Logger;


/**
 * Manager class represents a connection to a given Socket.IO server.
 */
public class Manager extends Emitter {

    private static final Logger logger = Logger.getLogger(Manager.class.getName());

    /*package*/ enum ReadyState {
        CLOSED, OPENING, OPEN
    }

    /**
     * Called on a successful connection.
     */
    public static final String EVENT_OPEN = "open";

    /**
     * Called on a disconnection.
     */
    public static final String EVENT_CLOSE = "close";

    public static final String EVENT_PACKET = "packet";
    public static final String EVENT_ERROR = "error";

    /**
     * Called on a connection error.
     */
    public static final String EVENT_CONNECT_ERROR = "connect_error";

    /**
     * Called on a connection timeout.
     */
    public static final String EVENT_CONNECT_TIMEOUT = "connect_timeout";

    /**
     * Called on a successful reconnection.
     */
    public static final String EVENT_RECONNECT = "reconnect";

    /**
     * Called on a reconnection attempt error.
     */
    public static final String EVENT_RECONNECT_ERROR = "reconnect_error";

    public static final String EVENT_RECONNECT_FAILED = "reconnect_failed";

    public static final String EVENT_RECONNECT_ATTEMPT = "reconnect_attempt";

    public static final String EVENT_RECONNECTING = "reconnecting";

    /**
     * Called when a new transport is created. (experimental)
     */
    public static final String EVENT_TRANSPORT = Engine.EVENT_TRANSPORT;

    /*package*/ static SSLContext defaultSSLContext;

    /*package*/ ReadyState readyState = null;

    private boolean _reconnection;
    private boolean skipReconnect;
    private boolean reconnecting;
    private boolean encoding;
    private boolean openReconnect;
    private int _reconnectionAttempts;
    private long _reconnectionDelay;
    private long _reconnectionDelayMax;
    private long _timeout;
    private int connected;
    private int attempts;
    private URI uri;
    private List<Packet> packetBuffer;
    private Queue<On.Handle> subs;
    private Options opts;
    /*package*/ com.github.nkzawa.engineio.client.Socket engine;
    private Parser.Encoder encoder;
    private Parser.Decoder decoder;

    /**
     * This HashMap can be accessed from outside of EventThread.
     */
    private ConcurrentHashMap<String, Socket> nsps;

    private ScheduledExecutorService timeoutScheduler;
    private ScheduledExecutorService reconnectScheduler;


    public Manager() {
        this(null, null);
    }

    public Manager(URI uri) {
        this(uri, null);
    }

    public Manager(Options opts) {
        this(null, opts);
    }

    public Manager(URI uri, Options opts) {
        if (opts == null) {
            opts = new Options();
        }
        if (opts.path == null) {
            opts.path = "/socket.io";
        }
        if (opts.sslContext == null) {
            opts.sslContext = defaultSSLContext;
        }
        this.opts = opts;
        this.nsps = new ConcurrentHashMap<String, Socket>();
        this.subs = new LinkedList<On.Handle>();
        this.reconnection(opts.reconnection);
        this.reconnectionAttempts(opts.reconnectionAttempts != 0 ? opts.reconnectionAttempts : Integer.MAX_VALUE);
        this.reconnectionDelay(opts.reconnectionDelay != 0 ? opts.reconnectionDelay : 1000);
        this.reconnectionDelayMax(opts.reconnectionDelayMax != 0 ? opts.reconnectionDelayMax : 5000);
        this.timeout(opts.timeout < 0 ? 20000 : opts.timeout);
        this.readyState = ReadyState.CLOSED;
        this.uri = uri;
        this.connected = 0;
        this.attempts = 0;
        this.encoding = false;
        this.packetBuffer = new ArrayList<Packet>();
        this.encoder = new Parser.Encoder();
        this.decoder = new Parser.Decoder();
    }

    private void emitAll(String event, Object... args) {
        this.emit(event, args);
        for (Socket socket : this.nsps.values()) {
            socket.emit(event, args);
        }
    }

    public boolean reconnection() {
        return this._reconnection;
    }

    public Manager reconnection(boolean v) {
        this._reconnection = v;
        return this;
    }

    public int reconnectionAttempts() {
        return this._reconnectionAttempts;
    }

    public Manager reconnectionAttempts(int v) {
        this._reconnectionAttempts = v;
        return this;
    }

    public long reconnectionDelay() {
        return this._reconnectionDelay;
    }

    public Manager reconnectionDelay(long v) {
        this._reconnectionDelay = v;
        return this;
    }

    public long reconnectionDelayMax() {
        return this._reconnectionDelayMax;
    }

    public Manager reconnectionDelayMax(long v) {
        this._reconnectionDelayMax = v;
        return this;
    }

    public long timeout() {
        return this._timeout;
    }

    public Manager timeout(long v) {
        this._timeout = v;
        return this;
    }

    private void maybeReconnectOnOpen() {
        if (!this.openReconnect && !this.reconnecting && this._reconnection) {
            this.openReconnect = true;
            this.reconnect();
        }
    }

    public Manager open(){
        return open(null);
    }

    /**
     * Connects the client.
     *
     * @param fn callback.
     * @return a reference to this object.
     */
    public Manager open(final OpenCallback fn) {
        EventThread.exec(new Runnable() {
            @Override
            public void run() {
                logger.fine(String.format("readyState %s", Manager.this.readyState));
                if (Manager.this.readyState == ReadyState.OPEN) return;

                logger.fine(String.format("opening %s", Manager.this.uri));
                Manager.this.engine = new Engine(Manager.this.uri, Manager.this.opts);
                final com.github.nkzawa.engineio.client.Socket socket = Manager.this.engine;
                final Manager self = Manager.this;

                Manager.this.readyState = ReadyState.OPENING;

                // propagate transport event.
                socket.on(Engine.EVENT_TRANSPORT, new Listener() {
                    @Override
                    public void call(Object... args) {
                        self.emit(Manager.EVENT_TRANSPORT, args);
                    }
                });

                final On.Handle openSub = On.on(socket, Engine.EVENT_OPEN, new Listener() {
                    @Override
                    public void call(Object... objects) {
                        self.onopen();
                        if (fn != null) fn.call(null);
                    }
                });

                On.Handle errorSub = On.on(socket, Engine.EVENT_ERROR, new Listener() {
                    @Override
                    public void call(Object... objects) {
                        Object data = objects.length > 0 ? objects[0] : null;
                        logger.fine("connect_error");
                        self.cleanup();
                        self.readyState = ReadyState.CLOSED;
                        self.emitAll(EVENT_CONNECT_ERROR, data);
                        if (fn != null) {
                            Exception err = new SocketIOException("Connection error",
                                    data instanceof Exception ? (Exception) data : null);
                            fn.call(err);
                        }

                        self.maybeReconnectOnOpen();
                    }
                });

                if (Manager.this._timeout >= 0) {
                    final long timeout = Manager.this._timeout;
                    logger.fine(String.format("connection attempt will timeout after %d", timeout));

                    final Future timer = getTimeoutScheduler().schedule(new Runnable() {
                        @Override
                        public void run() {
                            EventThread.exec(new Runnable() {
                                @Override
                                public void run() {
                                    logger.fine(String.format("connect attempt timed out after %d", timeout));
                                    openSub.destroy();
                                    socket.close();
                                    socket.emit(Engine.EVENT_ERROR, new SocketIOException("timeout"));
                                    self.emitAll(EVENT_CONNECT_TIMEOUT, timeout);
                                }
                            });
                        }
                    }, timeout, TimeUnit.MILLISECONDS);

                    Manager.this.subs.add(new On.Handle() {
                        @Override
                        public void destroy() {
                            timer.cancel(false);
                        }
                    });
                }

                Manager.this.subs.add(openSub);
                Manager.this.subs.add(errorSub);

                Manager.this.engine.open();
            }
        });
        return this;
    }

    private void onopen() {
        logger.fine("open");

        this.cleanup();

        this.readyState = ReadyState.OPEN;
        this.emit(EVENT_OPEN);

        final com.github.nkzawa.engineio.client.Socket socket = this.engine;
        this.subs.add(On.on(socket, Engine.EVENT_DATA, new Listener() {
            @Override
            public void call(Object... objects) {
                Object data = objects[0];
                if (data instanceof String) {
                    Manager.this.ondata((String)data);
                } else if (data instanceof byte[]) {
                    Manager.this.ondata((byte[])data);
                }
            }
        }));
        this.subs.add(On.on(this.decoder, Parser.Decoder.EVENT_DECODED, new Listener() {
            @Override
            public void call(Object... objects) {
                Manager.this.ondecoded((Packet) objects[0]);
            }
        }));
        this.subs.add(On.on(socket, Engine.EVENT_ERROR, new Listener() {
            @Override
            public void call(Object... objects) {
                Manager.this.onerror((Exception)objects[0]);
            }
        }));
        this.subs.add(On.on(socket, Engine.EVENT_CLOSE, new Listener() {
            @Override
            public void call(Object... objects) {
                Manager.this.onclose((String)objects[0]);
            }
        }));
    }

    private void ondata(String data) {
        this.decoder.add(data);
    }

    private void ondata(byte[] data) {
        this.decoder.add(data);
    }

    private void ondecoded(Packet packet) {
        this.emit(EVENT_PACKET, packet);
    }

    private void onerror(Exception err) {
        logger.log(Level.FINE, "error", err);
        this.emitAll(EVENT_ERROR, err);
    }

    /**
     * Initializes {@link Socket} instances for each namespaces.
     *
     * @param nsp namespace.
     * @return a socket instance for the namespace.
     */
    public Socket socket(String nsp) {
        Socket socket = this.nsps.get(nsp);
        if (socket == null) {
            socket = new Socket(this, nsp);
            Socket _socket = this.nsps.putIfAbsent(nsp, socket);
            if (_socket != null) {
                socket = _socket;
            } else {
                final Manager self = this;
                socket.on(Socket.EVENT_CONNECT, new Listener() {
                    @Override
                    public void call(Object... objects) {
                        self.connected++;
                    }
                });
            }
        }
        return socket;
    }

    /*package*/ void destroy(Socket socket) {
        --this.connected;
        if (this.connected == 0) {
            this.close();
        }
    }

    /*package*/ void packet(Packet packet) {
        logger.fine(String.format("writing packet %s", packet));
        final Manager self = this;

        if (!self.encoding) {
            self.encoding = true;
            this.encoder.encode(packet, new Parser.Encoder.Callback() {
                @Override
                public void call(Object[] encodedPackets) {
                    for (Object packet : encodedPackets) {
                        if (packet instanceof String) {
                            self.engine.write((String)packet);
                        } else if (packet instanceof byte[]) {
                            self.engine.write((byte[])packet);
                        }
                    }
                    self.encoding = false;
                    self.processPacketQueue();
                }
            });
        } else {
            self.packetBuffer.add(packet);
        }
    }

    private void processPacketQueue() {
        if (this.packetBuffer.size() > 0 && !this.encoding) {
            Packet pack = this.packetBuffer.remove(0);
            this.packet(pack);
        }
    }

    private void cleanup() {
        On.Handle sub;
        while ((sub = this.subs.poll()) != null) sub.destroy();
    }

    private void close() {
        this.skipReconnect = true;
        this.engine.close();
    }

    private void onclose(String reason) {
        logger.fine("close");
        this.cleanup();
        this.readyState = ReadyState.CLOSED;
        this.emit(EVENT_CLOSE, reason);

        if (this.timeoutScheduler != null) {
            this.timeoutScheduler.shutdown();
        }
        if (this.reconnectScheduler != null) {
            this.reconnectScheduler.shutdown();
        }

        if (this._reconnection && !this.skipReconnect) {
            this.reconnect();
        }
    }

    private void reconnect() {
        if (this.reconnecting) return;

        final Manager self = this;
        this.attempts++;

        if (attempts > this._reconnectionAttempts) {
            logger.fine("reconnect failed");
            this.emitAll(EVENT_RECONNECT_FAILED);
            this.reconnecting = false;
        } else {
            long delay = this.attempts * this.reconnectionDelay();
            delay = Math.min(delay, this.reconnectionDelayMax());
            logger.fine(String.format("will wait %dms before reconnect attempt", delay));

            this.reconnecting = true;
            final Future timer = this.getReconnectScheduler().schedule(new Runnable() {
                @Override
                public void run() {
                    EventThread.exec(new Runnable() {
                        @Override
                        public void run() {
                            logger.fine("attempting reconnect");
                            self.emitAll(EVENT_RECONNECT_ATTEMPT, self.attempts);
                            self.emitAll(EVENT_RECONNECTING, self.attempts);
                            self.open(new OpenCallback() {
                                @Override
                                public void call(Exception err) {
                                    if (err != null) {
                                        logger.fine("reconnect attempt error");
                                        self.reconnecting = false;
                                        self.reconnect();
                                        self.emitAll(EVENT_RECONNECT_ERROR, err);
                                    } else {
                                        logger.fine("reconnect success");
                                        self.onreconnect();
                                    }
                                }
                            });
                        }
                    });
                }
            }, delay, TimeUnit.MILLISECONDS);

            this.subs.add(new On.Handle() {
                @Override
                public void destroy() {
                    timer.cancel(false);
                }
            });
        }
    }

    private void onreconnect() {
        int attempts = this.attempts;
        this.attempts = 0;
        this.reconnecting = false;
        this.emitAll(EVENT_RECONNECT, attempts);
    }

    private ScheduledExecutorService getTimeoutScheduler() {
        if (this.timeoutScheduler == null || this.timeoutScheduler.isShutdown()) {
           this.timeoutScheduler = Executors.newSingleThreadScheduledExecutor();
        }
        return timeoutScheduler;
    }

    private ScheduledExecutorService getReconnectScheduler() {
        if (this.reconnectScheduler == null || this.reconnectScheduler.isShutdown()) {
            this.reconnectScheduler = Executors.newSingleThreadScheduledExecutor();
        }
        return this.reconnectScheduler;
    }


    public static interface OpenCallback {

        public void call(Exception err);
    }


    private static class Engine extends com.github.nkzawa.engineio.client.Socket {

        Engine(URI uri, Options opts) {
            super(uri, opts);
        }
    }

    public static class Options extends com.github.nkzawa.engineio.client.Socket.Options {

        public boolean reconnection = true;
        public int reconnectionAttempts;
        public long reconnectionDelay;
        public long reconnectionDelayMax;
        public long timeout = -1;
    }
}
