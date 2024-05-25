## NOTE: use same radar chart, but for interdists
def OLD_visual_speeds(t_speeds, speed_stats):

    ## get on type data
    t_avgs, t_stds, t_qt1s, t_qt2s, t_qt3s = zip(*[(s.avg, s.std, s.qts[0], s.qts[1], s.qts[2]) for s in speed_stats.typed])
    t_maxs = [speeds[-1] for speeds in t_speeds]

    ## get on all data
    a_avg, a_std, (a_qt1, a_qt2, a_qt3) = speed_stats.all
    a_max = t_maxs[-1]

    ## setup figure and axes
    fig, axs = plt.subplots(1, 3, subplot_kw={"projection": "polar"}, figsize=(18, 6))
    axs[0].set_title("General")
    axs[1].set_title("Standard Deviations")
    axs[2].set_title("Minimums")
    
    # ## TYPED GENERAL STATS

    labels = ["Type Mean", "Type Q1", "Type Q2", "Type Q3"]
    groups = [ f"Type {t}" for t in range(T) ] + ["All"]
    data = []
    for s in speed_stats.typed:
        data.append([s.avg, s.qts[0], s.qts[1], s.qts[2]])

    angles = [ 2*pi * n/4 for n in range(4) ]
    angles += angles[:1]

    axs[0].set_theta_offset(pi / 2)
    axs[0].set_theta_direction(-1)

    axs[0].set_xticks(angles[:-1], labels)

    axs[0].set_ylim(0, a_max)
    axs[0].set_rlabel_position(0)
    axs[0].set_yticks([a_avg, a_qt1, a_qt2, a_qt3], ["Mean", "Q1", "Q2", "Q3"], color="grey", size=7)

    for t, d in enumerate(data):
        d += [d[0]]
        axs[0].plot(angles, d, linewidth=1, linestyle="solid", label=groups[t], color=particle_colours[t])
    
    plt.show()