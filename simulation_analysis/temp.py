def OLD_visualise_intercounts(t_interdists, interdist_stats):

    typed_intercounts = [ [ len(dists) for dists in t1dists ] for t1dists in t_interdists ]
    for tis in typed_intercounts:
        tis.reverse()

    print()
    for ti in typed_intercounts:
        print(ti)
    
    fig = plt.figure()
    ax = fig.add_subplot(projection="3d")
    ax.view_init(azim=-135, elev=45)

    ticks = range(T)

    ## cross hatch 1
    for colour, ys, tick in zip(particle_colours, typed_intercounts, ticks):
        xs = np.arange(T)
        ax.bar(xs, ys, zs=tick, zdir='y', color=colour, alpha=0.8)
    
    ## cross hatch 2
    for colour, ys, tick in zip(particle_colours, zip(*typed_intercounts), ticks):
        xs = np.arange(T)
        ax.bar(xs, ys, zs=tick, zdir='x', color=colour, alpha=0.8)

    ax.set_xlabel("Type 1")
    ax.set_ylabel("Type 2")
    ax.set_zlabel("Interactions")

    ax.set_yticks(ticks)

    plt.show()
    