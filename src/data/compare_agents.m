function compare_agents()
    clc, clear, close all

    types = {'multi-agents'};
    envs_ = {'maze', 'prey'};
    methods_ = {'sep', 'refmat'};
    action_pickers = {'boltzmann', 'greedy'};

    selected_type = types{1};
    selected_action_picker_index = 2;
    selected_action_picker = action_pickers{selected_action_picker_index};

    for e = 1:size(envs_, 2)
        data = [];
        legends = { };
        selected_env = envs_{e};
        close, open_hidden_fig(), hold on;
        m = {'-','--','-.','-+','-*','-o','^','v','>','<','p','h'};
        j = 1;
        for i =1:size(methods_, 2)
            path_ = sprintf('%s/%s/%s/env/%s', selected_action_picker, selected_type, methods_{i}, envs_{e});
            ll = dir(sprintf('%s/*.mat', path_));
            for li = 1:size(ll, 1)
                data(end+1, :) = load(sprintf('%s/%s', path_, ll(li).name), '-ascii');
                agents = regexprep(regexp(ll(li).name, '--agents \d+', 'match'), '--agents ', '');
                legends{end+1} = sprintf('%s %s (%.2f)\n', upper(methods_{i}), agents{1}, min(data(end)));
            end
        end
        plot(data', 'LineWidth', 1)
        grid;
        legend(legends, 'Location', 'northeast')
        xlabel('Trials')
        ylabel('Avg. Moves');
        title(sprintf('The effect of agents# on learning speed and prefomance over `%s` env with `%s` as action picker method.', selected_env, selected_action_picker));
        path_ = sprintf('%s', selected_action_picker);
        savefig(sprintf('%s/%s.multi-agent.pref.compare.fig', path_, selected_env))
        print(sprintf('%s/%s.multi-agent.pref.compare.png', path_, selected_env), '-r300', '-dpng')
    end


function open_hidden_fig()
    % figure('units','normalized','outerposition',[0 0 1 1])
    set(gcf, 'Visible', 'off')
    m = {'-','--','*','x','+','-.','^','v','>','<','p','h'};
    set(gca, 'LineStyleOrder',m, 'NextPlot','replacechildren', 'LineWidth', 2)