#!/usr/bin/python

import json
import os

INTER_FILE = "intersections.json"
TRONC_FILE = "troncons.json"

TEST = '{\n\
"type": "FeatureCollection",\n\
"features": [\n\
{ "type": "Feature", "properties": { "nom": "", "commune": "", "codefuv": "", "importance": "", "domanialite": "", "nom2": "", "commune2": "", "codefuv2": "", "gid": "1" }, "geometry": { "type": "Point", "coordinates": [ 4.83466888161576, 45.773806965343859 ] } },\n\
{ "type": "Feature", "properties": { "nom": "", "commune": "", "codefuv": "", "importance": "", "domanialite": "", "nom2": "", "commune2": "", "codefuv2": "", "gid": "2" }, "geometry": { "type": "Point", "coordinates": [ 4.835887003592073, 45.774775541072231 ] } }\n\
]\n\
}'

CONST_ARRON = 1000000000000000

class Road:
	'''Direction : 0 == Both directions
					1 == Logical direction
					2 == Inverse direction 
	'''
	def __init__(self, distance, points, direction):
		self.distance = float(distance);
		self.points = points;
		self.direction = direction

class Neighbor:
	def __init__(self, pathNode, road):
		self.neighbor = pathNode;
		self.road = road;

class PathNode:
	def __init__(self, point):
		self.point = point;
		self.neighbors = [];

	def add(self, neighbor):
		self.neighbors.append(neighbor);

class Point:
	def __init__(self, longitude, latitude):
		self.longitude = float(longitude)
		self.latitude = float(latitude)

	def __str__(self):
		return '{}, {}'.format(self.longitude, self.latitude)

def main() :
	
	pathInter = os.path.join(os.getcwd(), INTER_FILE)
	with open(pathInter, 'r') as infile:
		obj = json.load(infile)

	nodes = {};

	for intersection in obj.get('features'):
		gid = intersection.get('properties').get('gid')

		coord = intersection.get('geometry').get('coordinates')
		longitude = coord[1]*CONST_ARRON
		latitude = coord[0]*CONST_ARRON
		nodes[(longitude, latitude)] = PathNode(Point(longitude, latitude))

	pathTronc = os.path.join(os.getcwd(), TRONC_FILE)
	with open(pathTronc, 'r') as infile:
		obj = json.load(infile)

	for troncon in obj.get('features'):
		
		gid = troncon.get('properties').get('gid')
		direction = troncon.get('properties').get('senscirc')
		if direction == 'Double':
			direction = 0
		elif direction == 'Conforme':
			direction = 1
		elif direction == 'Inverse':
			direction = 2
		else:
			print 'Error when parsing direction'

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
		
		'''TODO distance to calculate'''
		distance = 0

		road = Road(distance, points, direction)

		if ok:
			n1 = Neighbor(end, road)
			n2 = Neighbor(start, road)
			start.add(n1)
			end.add(n2)

if __name__ == "__main__":
	main()
