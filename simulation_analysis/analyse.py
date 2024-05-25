import pandas as pd
from collections import namedtuple
import matplotlib.pyplot as plt
import math



DIR_NAME = "./data/"

JSON_NAME = "small_boy_seeded(30000).json"

RAW_DATA = pd.read_json(DIR_NAME+JSON_NAME)

T = RAW_DATA["simulation"]["launchSettings"]["types"]
N = RAW_DATA["simulation"]["launchSettings"]["count"]


Particle = namedtuple("Particle", ["t", "x", "y", "vx", "vy"])



def get_particles_by_type(particles_data):
    typed_particles = [[] for _ in range(T)]
    for p_data in particles_data:
        t = p_data["t"]
        typed_particles[t].append( Particle(t, p_data["x"], p_data["y"], p_data["vx"], p_data["vy"]) )
    return typed_particles



def get_typed_speeds(typed_particles):
    return [ [ math.sqrt(p.vx**2 + p.vy**2) for p in particles ] for particles in typed_particles ]



def get_typed_interdists(typed_particles):
    interdists = [ [ [] for _ in range(T) ] for _ in range(T) ]

    for t1, particles1 in enumerate(typed_particles):
        for t2, particles2 in enumerate(typed_particles):
            for p1 in particles1:
                for p2 in particles2:
                    if p1 != p2:
                        dist = math.sqrt((p1.x - p2.x)**2 + (p1.y - p2.y)**2)
                        if dist <= 2:
                            interdists[t1][t2].append(dist)

    return interdists



def get_inner_outer_interdists(typed_interdists):
    inner_radius = RAW_DATA["simulation"]["launchSettings"]["innerRadius"]
    inner_interdists = [ [ [] for _ in range(T) ] for _ in range(T) ]
    outer_interdists = [ [ [] for _ in range(T) ] for _ in range(T) ]

    for t1, t1dists in enumerate(typed_interdists):
        for t2, dists in enumerate(t1dists):
            for d in dists:
                if d < inner_radius:
                    inner_interdists[t1][t2].append(d)
                else:
                    outer_interdists[t1][t2].append(d)
    
    return inner_interdists, outer_interdists



def main():

    ## TODO: comparison of originals and resulting

    printing = True
    plotting = False

    typed_particles = get_particles_by_type(RAW_DATA["simulation"]["particles"])

    typed_speeds = get_typed_speeds(typed_particles)

    typed_interdists = get_typed_interdists(typed_particles)

    typed_inner_interdists, typed_outer_interdists = get_inner_outer_interdists(typed_interdists)


    if printing:

        print("\nAvg Speeds:")
        for t, speeds in enumerate(typed_speeds):
            avg = sum(speeds) / len(speeds)
            print(f" t{t}: {avg}")
        total_avg = sum([sum(speeds) for speeds in typed_speeds]) / sum([len(speeds) for speeds in typed_speeds])
        print(f"T: {total_avg}")

        print("\nInteractions:", end="")
        for t1, t1dists in enumerate(typed_interdists):
            print(f"\nt{t1}: ", end="")
            for dists in t1dists:
                print(len(dists), end=" ")
        print()

        print("\nInner Interactions:", end="")
        for t1, t1dists in enumerate(typed_inner_interdists):
            print(f"\nt{t1}: ", end="")
            for dists in t1dists:
                print(len(dists), end=" ")
        print()

        print("\nOuter Interactions:", end="")
        for t1, t1dists in enumerate(typed_outer_interdists):
            print(f"\nt{t1}: ", end="")
            for dists in t1dists:
                print(len(dists), end=" ")
        print()

    if plotting:
        pass


if __name__ == "__main__":
    main()
