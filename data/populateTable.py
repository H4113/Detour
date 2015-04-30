#!/usr/bin/python

import json
import re

OUTPUT = "pop.sql"
JSON_FILE = "tourisme.json"

def formatstring(s):
    return re.sub(r"'", r"''", s)

with open(JSON_FILE, "r") as g:
    data = json.loads(g.read())
    with open(OUTPUT, "w") as f:
        f.write("DELETE FROM tourism;\n")
        f.write("INSERT INTO tourism(id, typ, typ_detail, nom, adresse, codepostal, commune, ouverture, longitude, latitude) VALUES\n")
        features = data.get("features")

        for i, feature in enumerate(features):
            prop = feature.get("properties")
            coords = feature.get("geometry").get("coordinates")
            t = (prop.get("id"), formatstring(prop.get("type")), formatstring(prop.get("type_detail")), formatstring(prop.get("nom")), formatstring(prop.get("adresse")), formatstring(prop.get("codepostal")), formatstring(prop.get("commune")), formatstring(prop.get("ouverture")), coords[1], coords[0])
            f.write("(%s, '%s', '%s', '%s', '%s', '%s', '%s', '%s', %s, %s)" % t)
            if i != len(features)-1:
                f.write(",")
            f.write("\n")
        f.write(";\n")

