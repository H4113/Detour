#!/usr/bin/python

import json
import os
import math

INTER_FILE = "intersections.json"
TRONC_FILE = "troncons.json"
ROADS_OUT_FILE = "roads.out"
NODES_OUT_FILE = "nodes.out"

CONST_ARRON = 1000000000000000
R_EARTH = 6371000

class Road:
	ids = 0

	'''Direction : 
			0 == Both directions
			1 == Logical direction
			2 == Inverse direction
	'''
	def __init__(self, distance, points, direction):
		self.distance = float(distance)
		self.points = points
		self.direction = direction
		self.id = Road.ids
		Road.ids += 1

	def __str__(self):
		string = "{},{},{},".format(self.id, self.distance, len(self.points))
		for p in self.points:
			string += str(p)+","
		string += "{}".format(self.direction)
		return string

class Neighbor:
	def __init__(self, pathNode, id_road):
		self.neighbor = pathNode
		self.id_road = id_road

	def __str__(self):
		string = "{},{}".format(self.neighbor.id, self.id_road)
		return string

class PathNode:
	ids = 0

	def __init__(self, point):
		self.point = point
		self.neighbors = []
		self.id = PathNode.ids
		PathNode.ids += 1

	def add(self, neighbor):
		self.neighbors.append(neighbor)

	def __str__(self):
		string = "{},{},{},".format(self.id, str(self.point), len(self.neighbors))
		for i, n in enumerate(self.neighbors):
			string += "{}".format(str(n))
			if i != len(self.neighbors)-1:
				string += ","
		return string

class Point:
	def __init__(self, longitude, latitude):
		self.longitude = float(longitude)
		self.latitude = float(latitude)

	def __str__(self):
		return '{},{}'.format(self.longitude/CONST_ARRON, self.latitude/CONST_ARRON)

def getDirection(direction):
	if direction == 'Double':
		return 0
	elif direction == 'Conforme':
		return 1
	elif direction == 'Inverse':
		return 2
	else:
		print 'Error when parsing direction'
		return -1

def getPhiAndTheta(pointA, pointB):
	latA = pointA.point.latitude/CONST_ARRON
	latB = pointB.point.latitude/CONST_ARRON
	longA = pointA.point.longitude/CONST_ARRON
	longB = pointB.point.longitude/CONST_ARRON

	degree_to_radians = math.pi / 180.

	phiA = (90. - latA) * degree_to_radians
	phiB = (90. - latB) * degree_to_radians
	
	thetaA = longA * degree_to_radians
	thetaB = longB * degree_to_radians

	return {'phiA' : phiA, 'thetaA' : thetaA, 'phiB' : phiB, 'thetaB' : thetaB}

def calculateDistance(pointA, pointB):
	
	if pointA == pointB:
		return 0.0
	else:
		res = getPhiAndTheta(pointA,pointB)
		phiA = res.get('phiA')
		phiB = res.get('phiB')
		thetaA = res.get('thetaA')
		thetaB = res.get('thetaB')

		cos = (math.sin(phiA)*math.sin(phiB)*math.cos(thetaA - thetaB) + math.cos(phiA)*math.cos(phiB))
		arc = math.acos(cos)

		return R_EARTH * arc

'''Gautier's way'''
def calculate2(pointA, pointB):
	res = getPhiAndTheta(pointA,pointB)
	phiA = res.get('phiA')
	phiB = res.get('phiB')
	thetaA = res.get('thetaA')
	thetaB = res.get('thetaB')

	return R_EARTH * math.sqrt(phiA * phiA + phiB * phiB - 2. * phiA * phiB * math.cos(thetaA - thetaB))

def parseFiles() :
	
	pathInter = os.path.join(os.getcwd(), INTER_FILE)
	with open(pathInter, 'r') as infile:
		obj = json.load(infile)

	nodes = {}
	roads = {}

	for intersection in obj.get('features'):
		coord = intersection.get('geometry').get('coordinates')
		longitude = coord[1]*CONST_ARRON
		latitude = coord[0]*CONST_ARRON
		nodes[(longitude, latitude)] = PathNode(Point(longitude, latitude))

	pathTronc = os.path.join(os.getcwd(), TRONC_FILE)
	with open(pathTronc, 'r') as infile:
		obj = json.load(infile)

	for troncon in obj.get('features'):
		direction = troncon.get('properties').get('senscirc')
		direction = getDirection(direction)

		list_coord = troncon.get('geometry').get('coordinates')

		points = []

		ok = True

		for i, coord in enumerate(list_coord):
			longitude = coord[1]*CONST_ARRON
			latitude = coord[0]*CONST_ARRON
			points.append(Point(longitude, latitude))
			if i == 0:
				start = nodes.get((longitude, latitude))
				if start == None:
					ok = False
			elif i == len(list_coord)-1:
				end = nodes.get((longitude, latitude))
				if end == None:
					ok = False
		if ok:
			distance = calculateDistance(start,end)
			road = Road(distance, points, direction)
			roads[road.id] = road

			n1 = Neighbor(end, road.id)
			n2 = Neighbor(start, road.id)
			start.add(n1)
			end.add(n2)

	return (nodes,roads)

def encodeNodes(nodes, roads):
	with open(NODES_OUT_FILE, 'w') as outfile:
		for n in nodes.itervalues():
			outfile.write(str(n)+"\n")

	with open(ROADS_OUT_FILE, 'w') as outfile:
		for r in roads.itervalues():
			outfile.write(str(r)+"\n")


if __name__ == "__main__":
	nodes = {}
	roads = {}
	res = parseFiles()
	nodes = res[0]
	roads = res[1]
	encodeNodes(nodes, roads)
