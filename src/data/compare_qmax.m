function compare_qmax()
    clc, clear, close all

    envs_ = {'maze', 'prey'};
    action_pickers = {'boltzmann', 'greedy'};

    selected_action_picker_index = 1;
    selected_action_picker = action_pickers{selected_action_picker_index};

    files_ = {
    {
        '/sep/env/prey/[randwalk] --iters 20 --env prey --method sep --agents 3 --rl-action-advisor boltzmann --output qtable-max --tau 1e+100.qtable.mat',
        '/sep/env/prey/--iters 20 --env prey --method sep --agents 3 --rl-action-advisor boltzmann --output qtable-max --tau 0.4.qtable.mat',
        '/sep/env/maze/[randwalk] --iters 20 --env maze --method sep --agents 3 --rl-action-advisor boltzmann --output qtable-max --tau 1e+100.qtable.mat',
        '/sep/env/maze/--iters 20 --env maze --method sep --agents 3 --rl-action-advisor boltzmann --output qtable-max --tau 0.4.qtable.mat',
        '/il/env/prey/[randwalk] --iters 20 --env prey --method il --agents 1 --rl-action-advisor boltzmann --output qtable-max --tau 1e+100.qtable.mat',
        '/il/env/prey/--iters 20 --env prey --method il --agents 1 --rl-action-advisor boltzmann --output qtable-max --tau 0.4.qtable.mat',
        '/il/env/maze/[randwalk] --iters 20 --env maze --method il --agents 1 --rl-action-advisor boltzmann --output qtable-max --tau 1e+100.qtable.mat',
        '/il/env/maze/--iters 20 --env maze --method il --agents 1 --rl-action-advisor boltzmann --output qtable-max --tau 0.4.qtable.mat',
        '/refmat/env/prey/--iters 20 --env prey --method refmat --output qtable-max --tau 0.4 --refmat-grind 4 --agents 3 --rl-action-advisor boltzmann --refmat-combinator fci-k-mean.qtable.mat',
        '/refmat/env/prey/[randwalk] --iters 20 --env prey --method refmat --output qtable-max --tau 1e+100 --refmat-grind 4 --agents 3 --rl-action-advisor boltzmann --refmat-combinator fci-k-mean.qtable.mat',
        '/refmat/env/maze/[randwalk] --iters 20 --env maze --method refmat --output qtable-max --tau 1e+100 --refmat-grind 3 --agents 3 --rl-action-advisor boltzmann --refmat-combinator fci-k-mean.qtable.mat',
        '/refmat/env/maze/--iters 20 --env maze --method refmat --output qtable-max --tau 0.4 --refmat-grind 3 --agents 3 --rl-action-advisor boltzmann --refmat-combinator fci-k-mean.qtable.mat',
    },
    {
        '/sep/env/prey/--iters 20 --env prey --method sep --agents 3 --epsilon 0.2 --rl-action-advisor greedy --output qtable-max --tau 0.4.qtable.mat',
        '/sep/env/prey/[randwalk] --iters 20 --env prey --method sep --agents 3 --epsilon 1 --rl-action-advisor greedy --output qtable-max --tau 0.4.qtable.mat',
        '/sep/env/maze/--iters 20 --env maze --method sep --agents 3 --epsilon 0.2 --rl-action-advisor greedy --output qtable-max --tau 0.4.qtable.mat',
        '/sep/env/maze/[randwalk] --iters 20 --env maze --method sep --agents 3 --epsilon 1 --rl-action-advisor greedy --output qtable-max --tau 0.4.qtable.mat',
        '/il/env/prey/--iters 20 --env prey --method il --agents 1 --epsilon 0.2 --rl-action-advisor greedy --output qtable-max --tau 0.4.qtable.mat',
        '/il/env/prey/[randwalk] --iters 20 --env prey --method il --agents 1 --epsilon 1 --rl-action-advisor greedy --output qtable-max --tau 0.4.qtable.mat',
        '/il/env/maze/--iters 20 --env maze --method il --agents 1 --epsilon 0.2 --rl-action-advisor greedy --output qtable-max --tau 0.4.qtable.mat',
        '/il/env/maze/[randwalk] --iters 20 --env maze --method il --agents 1 --epsilon 1 --rl-action-advisor greedy --output qtable-max --tau 0.4.qtable.mat',
        '/refmat/env/prey/--iters 20 --env prey --method refmat --output qtable-max --epsilon 0.2 --tau 0.4 --refmat-grind 4 --agents 3 --rl-action-advisor greedy --refmat-combinator fci-k-mean.qtable.mat',
        '/refmat/env/prey/[randwalk] --iters 20 --env prey --method refmat --output qtable-max --epsilon 1 --tau 0.4 --refmat-grind 4 --agents 3 --rl-action-advisor greedy --refmat-combinator fci-k-mean.qtable.mat',
        '/refmat/env/maze/[randwalk] --iters 20 --env maze --method refmat --output qtable-max --epsilon 1 --tau 0.4 --refmat-grind 3 --agents 3 --rl-action-advisor greedy --refmat-combinator fci-k-mean.qtable.mat',
        '/refmat/env/maze/--iters 20 --env maze --method refmat --output qtable-max --epsilon 0.2 --tau 0.4 --refmat-grind 3 --agents 3 --rl-action-advisor greedy --refmat-combinator fci-k-mean.qtable.mat',
    }};
    % 1: boltzmann
    % 2: greedy
    files = files_{selected_action_picker_index};
    for e = 1:size(envs_, 2)
        data = [];
        legends = { };
        selected_env = envs_{e};
        close, open_hidden_fig(), hold on;
        m = {'-','--','-.','-+','-*','-o','^','v','>','<','p','h'};

        j = 1;
        for i =1:length(files)
            if(length(regexp(files{i}, sprintf('--env %s', selected_env), 'match')) == 0), continue; end
            data(end+1, :) = mean(load(sprintf('%s/qtable-max%s', selected_action_picker, files{i}), '-ascii'));
            legends{end + 1} = '';
            if(length(regexp(files{i}, '\[randwalk\]', 'match')) > 0)
                legends{end} = 'RAND-WALK ';
            end
            k = regexprep(regexp(files{i}, '--method \w+', 'match'), '--method ', '');
            legends{end} = sprintf('%s%s', legends{end}, upper(k{1}));
            plot(data(end, :)', m{j}, 'Color', rand(1, 3), 'LineWidth', 1)
            j = j + 1;
        end
        grid;
        legend(legends, 'Location', 'northwest')
        xlabel('Trials')
        ylabel('Avg. Moves');
        path_ = sprintf('%s', selected_action_picker);
        savefig(sprintf('%s/%s.qtable.max.compare.fig', path_, selected_env))
        print(sprintf('%s/%s.qtable.max.compare.png', path_, selected_env), '-r300', '-dpng')
    end


function open_hidden_fig()
    figure('units','normalized','outerposition',[0 0 1 1])
    set(gcf, 'Visible', 'off')
    m = {'-','--','*','x','+','-.','^','v','>','<','p','h'};
    set(gca, 'LineStyleOrder',m, 'NextPlot','replacechildren', 'LineWidth', 2)