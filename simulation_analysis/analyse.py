import pandas as pd
from collections import namedtuple
import matplotlib.pyplot as plt
import math



DIR_NAME = "./data/"

JSON_NAME = "small_boy_seeded(30000).json"

RAW_DATA = pd.read_json(DIR_NAME+JSON_NAME)

T = RAW_DATA["simulation"]["launchSettings"]["types"]
N = RAW_DATA["simulation"]["launchSettings"]["count"]


Particle = namedtuple("Particle", [ "t", "x", "y", "vx", "vy" ])

StateData = namedtuple("StateData", [
    "particles",
    "typed_speeds",
    "typed_interdists",
    "typed_inner_interdists",
    "typed_outer_interdists"
])

Stats = namedtuple("Stats", [ "avg", "qts", "std" ])

DataStats = namedtuple("SpeedStats", [ "typed", "total" ])

StateStats = namedtuple("StateStats", [ "speeds", "interdists", "inner_interdists", "outer_interdists" ])



def get_typed_particles(particles_data):
    typed_particles = [[] for _ in range(T)]
    for p_data in particles_data:
        t = p_data["t"]
        typed_particles[t].append( Particle(t, p_data["x"], p_data["y"], p_data["vx"], p_data["vy"]) )
    return typed_particles



def get_typed_speeds(typed_particles):
    return [ sorted([ math.sqrt( p.vx**2 + p.vy**2 ) for p in particles ]) for particles in typed_particles ]


def get_typed_interdists(typed_particles):
    typed_interdists = [ [ [] for _ in range(T) ] for _ in range(T) ]

    for t1, particles1 in enumerate(typed_particles):
        for t2, particles2 in enumerate(typed_particles):
            for p1 in particles1:
                for p2 in particles2:
                    if p1 != p2:
                        dist = math.sqrt((p1.x - p2.x)**2 + (p1.y - p2.y)**2)
                        if dist <= 2:
                            typed_interdists[t1][t2].append(dist)

    for interdists in typed_interdists:
        for dists in interdists:
            dists.sort()
    
    return typed_interdists



def get_typed_inner_outer_interdists(typed_interdists):
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



def get_state_data(state_raw_data):
    particles = get_typed_particles(state_raw_data)
    speeds = get_typed_speeds(particles)
    interdists = get_typed_interdists(particles)
    inner_interdists, outer_interdists = get_typed_inner_outer_interdists(interdists)
    return StateData(particles, speeds, interdists, inner_interdists, outer_interdists)



def print_state_data(state_data):

    print("\n  Avg Speeds:")
    for t, speeds in enumerate(state_data.typed_speeds):
        avg = sum(speeds) / len(speeds)
        print(f"   t{t}: {avg}")
    total_avg = sum([sum(speeds) for speeds in state_data.typed_speeds]) / sum([len(speeds) for speeds in state_data.typed_speeds])
    print(f"   T: {total_avg}")

    print("\n  Interactions:", end="")
    for t1, t1dists in enumerate(state_data.typed_interdists):
        print(f"\n   t{t1}: ", end="")
        for dists in t1dists:
            print(len(dists), end=" ")
    print()

    print("\n  Inner Interactions:", end="")
    for t1, t1dists in enumerate(state_data.typed_inner_interdists):
        print(f"\n   t{t1}: ", end="")
        for dists in t1dists:
            print(len(dists), end=" ")
    print()

    print("\n  Outer Interactions:", end="")
    for t1, t1dists in enumerate(state_data.typed_outer_interdists):
        print(f"\n   t{t1}: ", end="")
        for dists in t1dists:
            print(len(dists), end=" ")
    print()



def get_speed_stats(typed_speeds):
    typed_stats = Stats([],[],[])
    total_stats = Stats([],[],[])



def main():

    ## TODO: comparison of originals and resulting

    isPrinting = True
    isPlotting = False

    launch_state_data = get_state_data(RAW_DATA["simulation"]["launchSettings"]["particles"])
    result_state_data = get_state_data(RAW_DATA["simulation"]["particles"])

    if isPrinting:

        print("\nLaunch State Data:")
        print_state_data(launch_state_data)
        print("\nResult State Data:")
        print_state_data(result_state_data)
        print()

    if isPlotting:
        pass


if __name__ == "__main__":
    main()


