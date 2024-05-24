import pandas as pd
from collections import namedtuple
import matplotlib.pyplot as plt
import math


Sim = namedtuple("SimParams", [
    "name",
    "frame_count",
    "types",
    "size",
    "count",
    "inner_radius",
    "resistance",
    "step",
    "attractions",
    "type_ratios",
    "seed",
    "originals",
    "resulting"
])

# PStat = namedtuple("PStat", [
#     "type",
#     "speed",
#     "inter_dists"
# ])
class Particle:
    def __init__(self, t, x, y, vx, vy):
        self.t = t
        self.x = x
        self.y = y
        self.vx = vx
        self.vy = vy
    def __str__(self):
        return "{"+f"t: {self.t}, x: {self.x}, y: {self.y}, vx: {self.vx}, vy: {self.vy}"+"}"
    def __repr__(self): return self.__str__()

class PStat:
    def __init__(self, speed, inter_dists):
        self.speed = speed
        self.inter_dists = inter_dists

DIR_NAME = "./data/"

fileName = "debug_seeded(10000).json"

T = 0
N = 0
type_counts = []


def read_json(file_path):
    data = pd.read_json(file_path)
    
    name = file_path[:-5]
    frame_count = data["simulation"]["frameCount"]
    types = data["simulation"]["launchSettings"]["types"]
    size = data["simulation"]["launchSettings"]["size"]
    count = data["simulation"]["launchSettings"]["count"]
    inner_radius = data["simulation"]["launchSettings"]["innerRadius"]
    resistance = data["simulation"]["launchSettings"]["resistance"]
    step = data["simulation"]["launchSettings"]["step"]
    attractions = data["simulation"]["launchSettings"]["attractions"]
    type_ratios = data["simulation"]["launchSettings"]["typeRatio"]
    seed = data["simulation"]["launchSettings"]["seed"]
    originals = data["simulation"]["launchSettings"]["particles"]
    resulting = data["simulation"]["particles"]

    return Sim(name, frame_count, types, size, count, inner_radius, resistance, step, attractions, type_ratios, seed, originals, resulting)

def count_types(particles):
    t = sim.types
    results = [0 for _ in range(t)]
    for p in particles:
        results[p["type"]] += 1
    return results

def get_ptype_data(data):
    dest = [[] for _ in range(T)]
    for p1data in data:
        t = p1data["type"]
        dest[t].append(Particle(t, p1data["xPos"], p1data["yPos"], p1data["xVel"], p1data["yVel"]))
    return dest

sim = read_json(DIR_NAME + fileName)
T = sim.types
N = sim.count
type_counts = count_types(sim.originals)

o_particles = [[] for _ in range(T)]
r_particles = [[] for _ in range(T)]

for i in range (N):
    op_data = sim.originals[i]
    rp_data = sim.resulting[i]
    o_particles[op_data["type"]].append(Particle(op_data["xPos"], op_data["yPos"], op_data["xVel"], op_data["yVel"]))
    r_particles[rp_data["type"]].append(Particle(rp_data["xPos"], rp_data["yPos"], rp_data["xVel"], rp_data["yVel"]))
# r_particles = get_ptype_data(sim.resulting)
# get_ptype_data(sim.resulting)
# print()
# for t in range(T):
#     print()
#     for p in r_particles[t]:
#         print(t, p)
