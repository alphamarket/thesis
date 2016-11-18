function compare_times()
    clc, clear, close all

    action_pickers = {'boltzmann', 'greedy'};

    selected_action_picker_index = 2;
    selected_action_picker = action_pickers{selected_action_picker_index};

    path_ = sprintf('%s', selected_action_picker)
    addpath(sprintf('%s/times', selected_action_picker))

    times_data

    close, open_hidden_fig();
    hold on, grid on
    plot(il_maze', 'LineWidth', 2);
    plot(sep_maze', 'r-.', 'LineWidth', 2);
    plot(refmat_maze', 'k-*', 'LineWidth', 2);
    xlabel('Trials')
    ylabel('Avg. Execution Time (ms)');
    legend('IL', 'SEP', 'REFMAT', 'Location', 'northwest');
    savefig(sprintf('%s/maze.time.compare.fig', path_))
    print(sprintf('%s/maze.time.compare.png', path_), '-r300', '-dpng')

    close, open_hidden_fig();
    hold on, grid on
    plot(il_prey', 'LineWidth', 2);
    plot(sep_prey', 'r-.', 'LineWidth', 2);
    plot(refmat_prey', 'k-*', 'LineWidth', 2);
    xlabel('Trials')
    ylabel('Avg. Execution Time (ms)');
    legend('IL', 'SEP', 'REFMAT', 'Location', 'northwest');
    savefig(sprintf('%s/prey.time.compare.fig', path_))
    print(sprintf('%s/prey.time.compare.png', path_), '-r300', '-dpng')

    rmpath(sprintf('%s/times', selected_action_picker));


function open_hidden_fig()
    figure('units','normalized','outerposition',[0 0 1 1])
    set(gcf, 'Visible', 'off')
    m = {'-','--','*','x','+','-.','^','v','>','<','p','h'};
    set(gca, 'LineStyleOrder',m, 'NextPlot','replacechildren', 'LineWidth', 2)