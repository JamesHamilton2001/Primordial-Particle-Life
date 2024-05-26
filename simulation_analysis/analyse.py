import pandas as pd
import numpy as np
import math
from math import pi
from collections import namedtuple
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches



DIR_NAME = "./data/"

JSON_NAME = "small_boy_seeded(30000).json"

RAW_DATA = pd.read_json(DIR_NAME+JSON_NAME)

T = RAW_DATA["simulation"]["launchSettings"]["types"]
N = RAW_DATA["simulation"]["launchSettings"]["count"]

type_radial_angles = [2 * pi * i / T for i in range(T)]
type_radial_angles += type_radial_angles[:1] # close circle


particle_colours = [
    (1.0, 0.0, 0.0),  # red
    (0.0, 0.0, 0.8),  # blue
    (0.8, 0.8, 0.0),  # yellow
    (0.5, 0.0, 0.5),  # purple
    (0.0, 0.8, 0.0),  # green
    (1.0, 0.5, 0.0),  # orange
    (1.0, 0.75, 0.8),  # pink
    (0.0, 1.0, 1.0),  # cyan
    (0.6, 0.4, 0.2)  # brown
]
particle_colours = particle_colours[:T]


Particle = namedtuple("Particle", [ "t", "x", "y", "vx", "vy" ])
StateData = namedtuple("StateData", [ "particles", "typed_speeds", "typed_interdists", "typed_inner_interdists", "typed_outer_interdists" ])
SimulationData = namedtuple("SimulationData", [ "launch", "result" ])


Stats = namedtuple("Stats", [ "avg", "std", "qts" ])
DataStats = namedtuple("DataStats", [ "typed", "all" ])
StateStats = namedtuple("StateStats", [ "speeds", "interdists", "inner_interdists", "outer_interdists" ])
SimulationStats = namedtuple("SimulationStats", [ "launch", "result" ])



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

    print("||Interactions:", end="")
    for t1, t1dists in enumerate(state_data.typed_interdists):
        print(f"\n|||t{t1}: ", end="")
        for dists in t1dists:
            print(len(dists), end=" ")

    print("\n||Inner_Interactions:", end="")
    for t1, t1dists in enumerate(state_data.typed_inner_interdists):
        print(f"\n|||t{t1}: ", end="")
        for dists in t1dists:
            print(len(dists), end=" ")

    print("\n||Outer_Interactions:", end="")
    for t1, t1dists in enumerate(state_data.typed_outer_interdists):
        print(f"\n|||t{t1}: ", end="")
        for dists in t1dists:
            print(len(dists), end=" ")
    print()

def print_simulation_data(simulation_data):
    print("|Launch:")
    print_state_data(simulation_data.launch)
    print("|Result:")
    print_state_data(simulation_data.result)



def get_stats(lst):
    
    slen = len(lst)
    if slen == 0:
        return Stats(0, 0, (0, 0, 0))

    avg = sum(lst) / slen
    std = math.sqrt(sum([ (x - avg)**2 for x in lst ]) / slen)

    if slen < 4:
        qts = (lst[0], lst[slen//2], lst[-1])
    else:
        qts = (lst[slen//4], lst[slen//2], lst[slen-slen//4])

    return Stats(avg, std, qts)

def get_speed_stats(typed_speeds):
    typed_stats = [get_stats(speeds) for speeds in typed_speeds]
    all_stats = get_stats([s for speeds in typed_speeds for s in speeds])
    return DataStats(typed_stats, all_stats)

def get_interdist_stats(typed_interdists):
    typed_stats = [ DataStats([get_stats(dists) for dists in t1dists], get_stats(sorted([d for t1ds in t1dists for d in t1ds]))) for t1dists in typed_interdists ]
    all_stats = get_stats([d for t1dists in typed_interdists for dists in t1dists for d in dists])
    return DataStats(typed_stats, all_stats)

def get_state_stats(state_data):
    speeds = get_speed_stats(state_data.typed_speeds)
    interdists = get_interdist_stats(state_data.typed_interdists)
    inner_interdists = get_interdist_stats(state_data.typed_inner_interdists)
    outer_interdists = get_interdist_stats(state_data.typed_outer_interdists)
    return StateStats(speeds, interdists, inner_interdists, outer_interdists)

def print_state_stats(state_stats):

    print("||Speed Stats:")
    for t, stats in enumerate(state_stats.speeds.typed):
        print(f"|||t{t}: {stats}")
    print(f"|||T: {state_stats.speeds.all}")

    print("||Interdist Stats:")
    for t1, t1stats in enumerate(state_stats.interdists.typed):
        for t2, stats in enumerate(t1stats.typed):
            print(f"|||t{t1}-{t2}: {stats}")
        print(f"|||T{t1}: {t1stats.all}")
    print(f"|||T: {state_stats.interdists.all}")

    print("||Inner Interdist Stats:")
    for t1, t1stats in enumerate(state_stats.inner_interdists.typed):
        for t2, stats in enumerate(t1stats.typed):
            print(f"|||t{t1}-{t2}: {stats}")
        print(f"|||T{t1}: {t1stats.all}")
    print(f"|||T: {state_stats.inner_interdists.all}")

    print("||Outer Interdist Stats:")
    for t1, t1stats in enumerate(state_stats.outer_interdists.typed):
        for t2, stats in enumerate(t1stats.typed):
            print(f"|||t{t1}-{t2}: {stats}")
        print(f"|||T{t1}: {t1stats.all}")
    print(f"|||T: {state_stats.outer_interdists.all}")

def print_simulation_stats(simulation_stats):
    print("|Launch:")
    print_state_stats(simulation_stats.launch)
    print("|Result:")
    print_state_stats(simulation_stats.result)
    print()



def visualise_speeds(t_speeds, speed_stats):
    
    plots = {
        "Standard Deviation": [s.std for s in speed_stats.typed],
        "Mean": [s.avg for s in speed_stats.typed],
        "Maximum": [speeds[-1] for speeds in t_speeds],
    }
    plots["Standard Deviation"].append(speed_stats.all.std)
    plots["Mean"].append(speed_stats.all.avg)
    plots["Maximum"].append(max(plots["Maximum"]))

    fig, ax = plt.subplots(figsize=(16, 9))
    
    x = np.arange(T+1)
    w = 0.25
    ax.set_title("Speeds Statistics")
    ax.set_ylabel("Speed")
    ax.set_xlabel("Type")
    ax.set_xticks(x + w, [f"{t}" for t in range(T)]+["All"])
    ax.set_ylim(0, 1.25 * plots["Maximum"][-1])

    alphas = [0.3, 0.5, 1]
    colours = particle_colours[:T] + [(0.5, 0.5, 0.5)]
    edge_colours = [(0.5, 0.5, 0.5, 1), (0.25, 0.25, 0.25, 1), (0, 0, 0, 1)]

    mult = 0
    for (label, data), a, edge_colour in zip(plots.items(), alphas, edge_colours):
        offset = w * mult
        bar_colours = [(c[0], c[1], c[2], a) for c in colours]
        rects = ax.bar(x + offset, data, w, label=label, color=bar_colours, edgecolor=edge_colour, linewidth=1)
        ax.bar_label(rects, labels=[format(d, ".6g") for d in data], label_type="edge", fontsize=10, padding=3)
        mult += 1

    for x_val, (q1, q2, q3) in zip(x, [s.qts for s in speed_stats.typed]+[speed_stats.all.qts]):
        xmin = x_val + 1.5*w
        xmax = xmin + w
        ax.fill_between([xmin, xmax], q1, q3, color=(0,0,0,0), edgecolor="black", linestyle="dashed")
        ax.hlines(q2, xmin=xmin, xmax=xmax, color="black", linestyle="dashed", linewidth=1)
        ax.text(xmax+0.03, q1, format(q1, ".6g"), ha="left", va="top", fontsize=10)
        ax.text(xmax+0.03, q2, format(q2, ".6g"), ha="left", va="center", fontsize=10)
        ax.text(xmax+0.03, q3, format(q3, ".6g"), ha="left", va="bottom", fontsize=10)

    colours = [(0.75,0.75,0.75),(0.55,0.55,0.55),(0.4,0.4,0.4)]
    legend_handles = [ mpatches.Patch(facecolor=colour, label=l, edgecolor="black") for colour, l in zip(colours, plots.keys())]
    legend_handles.append(mpatches.Patch(facecolor="white", edgecolor="black", linestyle="dashed", label="Quartiles"))
    ax.legend(handles=legend_handles, loc="upper right", ncols=4)
    
    plt.show()

def visualise_intercounts(t_interdists, interdist_stats):

    typed_intercounts = [ [ len(dists) for dists in t1dists ] for t1dists in t_interdists ]

    fig = plt.figure(figsize=(10, 10))
    ax = fig.add_subplot(projection="3d")

    ax.set_title("Total T2T Interactions", color="white")
    
    ax.view_init(azim=-135, elev=45)
    ax.set_box_aspect(None, zoom=0.8)

    plt.style.use('dark_background')
    ax.set_facecolor("black")
    ax.xaxis.pane.fill = False
    ax.yaxis.pane.fill = False
    ax.zaxis.pane.fill = False
    ax.xaxis.pane.set_edgecolor("w")
    ax.yaxis.pane.set_edgecolor("w")
    ax.zaxis.pane.set_edgecolor((0,0,0,0))

    ax.set_xlabel("XType 2", labelpad=15, color="white")
    ax.set_ylabel("YType 1", labelpad=15, color="white")
    ax.set_zlabel("Interactions", labelpad=15, color="white")

    wd = 0.4
    a = 0.4
    xyticks = np.arange(T)

    xyticks = np.arange(T)
    for xt, c1, tdata in zip(xyticks, particle_colours, typed_intercounts):
        for yt, c2, h in zip(xyticks, particle_colours, tdata):
            colour = ((c1[0]+c2[0])/2, (c1[1]+c2[1])/2, (c1[2]+c2[2])/2, a)
            ax.bar3d(xt+0.5-wd/2, yt+0.5-wd/2, 0, wd, wd, h, color=colour, linestyle="solid", edgecolor="black", linewidth=0.5)

    zticks = ax.get_zticks()

    ax.set_xlim(0, T)
    ax.set_ylim(0, T)
    ax.set_zlim(0, zticks[-1])
    ax.set_xticks(xyticks)
    ax.set_yticks(xyticks)
    zticks = zticks[1:-1]
    ax.set_zticks(zticks)
    ax.set_zticklabels([f"{t}" for t in zticks], color="white", fontsize=8)
    
    zt = zticks[0]
    for pos in range(T):
        ax.text(pos+0.5, -0.5, -0.25*zt, f"T{pos}", "x", color="white", fontsize=8, ha="center", va="center")
        ax.text(-0.5, pos+0.5, -0.25*zt, f"T{pos}", "y", color="white", fontsize=8, ha="center", va="center")

    for xt, tdata in zip(xyticks, typed_intercounts):
        for yt, h in zip(xyticks, tdata):
            ax.text(xt+0.5, yt+0.5, h+0.1*zt, f"{h}", color="white", fontsize=7, ha="center", va="center", zorder=99999)
    
    plt.show()



def visualise_interdists(t_interdists, interdist_stats):
    ## TODO: move counts to own function, use 3d bar chart with 2d data
    ## counts, overlap each and all for t, allall
    ## means, overlap each and all for t, all all
    ## stds, overlap each and all for t, all all
    ## quartiles ???

    t_angles = [ 2*pi * t/T for t in range(T) ] + [0]

    fig, axs = plt.subplots(2, 3, subplot_kw={"projection": "polar"}, figsize=(16, 9))
    fig, ax = plt.subplots(subplot_kw={"projection": "polar"}, figsize=(16, 9))

    t_labels = [ f"Type {t}" for t in range(T) ]
    t_groups = [ f"Type {t}" for t in range(T) ]

    for ax in axs.flat:
        ax.set_theta_offset(pi / 2)
        ax.set_theta_direction(-1)

    ## COUNTS

    typed_intercounts = [ [ len(dists) for dists in t1dists ] for t1dists in t_interdists ]
    total_intercounts_flat_sorted = sorted([ count for t1count in typed_intercounts for count in t1count ])
    total_intercount_stats = get_stats(total_intercounts_flat_sorted)
    max_intercount = total_intercounts_flat_sorted[-1]

    # ax = axs[0][0]
    ax.set_title("Total T2T Interactions")
    ax.set_xticks(t_angles[:-1], t_labels)
    ax.set_yticks(list(total_intercount_stats.qts), ["Q1", "Q2", "Q3"], color="grey", size=7)
    ax.set_ylim(0, 1.25 * max_intercount)

    for data, group, colour in zip(typed_intercounts, t_groups, particle_colours):
        data += [data[0]]
        print(data)
        ax.plot(t_angles, data, linewidth=1, linestyle="solid", label=group, color=colour)

    plt.show()
    


def main():

    isPrintingData = False
    isPrintingStats = False
    isPlotting = True

    simulation_state_data = SimulationData( get_state_data(RAW_DATA["simulation"]["launchSettings"]["particles"]),
                                            get_state_data(RAW_DATA["simulation"]["particles"])                  )
    
    simulation_state_stats = SimulationStats( get_state_stats(simulation_state_data.launch),
                                              get_state_stats(simulation_state_data.result) )

    if isPrintingData:
        print("\nSimulation Data:")
        print_simulation_data(simulation_state_data)
    if isPrintingStats:
        print("\nSimulation Stats:")
        print_simulation_stats(simulation_state_stats)

    if isPlotting:

        # visualise_speeds(simulation_state_data.result.typed_speeds, simulation_state_stats.result.speeds)
        visualise_intercounts(simulation_state_data.result.typed_interdists, simulation_state_stats.result.interdists)


if __name__ == "__main__":
    main()


