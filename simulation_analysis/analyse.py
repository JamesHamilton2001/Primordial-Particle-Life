import pandas as pd
from collections import namedtuple
import matplotlib.pyplot as plt
import math



DIR_NAME = "./data/"

JSON_FILE_NAME = "small_boy_seeded(30000).json"

RAW_DATA = pd.read_json(DIR_NAME+JSON_FILE_NAME)

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
    step = RAW_DATA["simulation"]["launchSettings"]["step"]
    return [ [ math.sqrt(p.vx**2 + p.vy**2) for p in particles ] for particles in typed_particles ]



def main():

    ## TODO: comparison of originals and resulting

    typed_particles = get_particles_by_type(RAW_DATA["simulation"]["particles"])

    typed_speeds = get_typed_speeds(typed_particles)

    print("\nAvg Speeds:")
    for t, speeds in enumerate(typed_speeds):
        avg = sum(speeds) / len(speeds)
        print(f" t{t}: {avg}")
    total_avg = sum([sum(speeds) for speeds in typed_speeds]) / sum([len(speeds) for speeds in typed_speeds])
    print(f"T: {total_avg}")




if __name__ == "__main__":
    main()
