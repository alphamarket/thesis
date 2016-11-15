function compare()
    clc, clear, close all

    action_pickers = {'boltzmann', 'greedy'};

    selected_action_picker = action_pickers{2};

    envs_ = {'prey', 'maze'};
    methods_ = {'fci-k-mean', 'fci-mean', 'fci-max', 'wsum', 'fci-const-one'};

    il_path = 'il';
    sep_path = 'sep';
    refmat_path = 'refmat';

    for e = 1:size(envs_, 2)
        min_methods = { };
        min_legends = { };
        for m = 1:size(methods_, 2)
            fprintf('> %s / %s / %s\n', selected_action_picker, envs_{e}, methods_{m});
            path_ = sprintf('%s/method/%s/env/%s/%s', selected_action_picker, refmat_path, envs_{e}, methods_{m});
            ll = dir(sprintf('%s/*.mat', path_));
            data = [];
            legend_ = {};
            min_ = inf;
            min_legends{end + 1} = '';
            min_methods{end + 1} = 0;
            for li = 1:size(ll, 1)
                data(end+1, :) = load(sprintf('%s/%s', path_, ll(li).name), '-ascii');
                legend_{end+1} = regexp(ll(li).name, '--refmat-grind \d+', 'match');
                legend_{end} = legend_{end}{1};
                if(data(end, end) < min_)
                    min_ = data(end, end);
                    min_methods{end} = data(end, :);
                    min_legends{end} = sprintf('--method %s %s (%.3f)', methods_{m}, legend_{end}, min_);
                end
            end
            close, open_hidden_fig()
            plot(data', 'LineWidth', 2), grid
            legend(legend_);
            title(sprintf('The grind comparison of `%s` method over `%s` with `%s` as action picker method', methods_{m}, envs_{e}, selected_action_picker));
            xlabel('Trials')
            ylabel('Avg. Moves');
            savefig(sprintf('%s/%s-%s-grind-compare.fig', path_, envs_{e}, methods_{m}))
            print(sprintf('%s/%s-%s-grind-compare.png', path_, envs_{e}, methods_{m}), '-r300', '-dpng')
        end
        % compare methods with each other
        data = [];
        for i = 1:size(min_methods, 2), data(end+1, :) = min_methods{i}; end
        close, open_hidden_fig(), plot(data', 'LineWidth', 2), grid;
        legend(min_legends)
        xlabel('Trials')
        ylabel('Avg. Moves');
        title(sprintf('The methods comparison over `%s` with `%s` as action picker method', methods_{m}, envs_{e}, selected_action_picker));
        path_ = sprintf('%s/method/%s/env/%s', selected_action_picker, refmat_path, envs_{e});
        savefig(sprintf('%s/%s-methods-compare.fig', path_, envs_{e}))
        print(sprintf('%s/%s-methods-compare.png', path_, envs_{e}), '-r300', '-dpng')

        % compare methods with SEP & IL
        agents = {3, 1};
        opt = {sep_path, il_path};
        for i = 1:size(opt, 2)
            path_ = sprintf('%s/method/%s/env/%s', selected_action_picker, opt{i}, envs_{e});
            data(end+1, :) = load(sprintf('%s/--iters 20 --env %s --method %s --agents %i.mat', path_, envs_{e}, opt{i}, agents{i}), '-ascii');
            min_legends{end+1} = upper(sprintf('%s (%.3f)', opt{i}, data(end, end)));
        end
        for i = 1:size(min_legends, 2)
            min_legends{i} = regexprep(min_legends{i}, '--method ([\w-]+) --refmat-grind \d+', 'REFMAT $1');
        end

        close, open_hidden_fig()
        plot(data', 'LineWidth', 2), grid;
        legend(min_legends)
        xlabel('Trials')
        ylabel('Avg. Moves');
        title(sprintf('The REFMAT/SEP/IL comparison over `%s` with `%s` as action picker method', envs_{e}, selected_action_picker));
        path_ = sprintf('%s', selected_action_picker);
        savefig(sprintf('%s/%s-compare.fig', path_, envs_{e}))
        print(sprintf('%s/%s-compare.png', path_, envs_{e}), '-r300', '-dpng')

    end

function open_hidden_fig()
    figure('units','normalized','outerposition',[0 0 1 1])
    set(gcf, 'Visible', 'off')
    m = {'-','--','*','x','+','-.','^','v','>','<','p','h'};
    set(gca, 'LineStyleOrder',m, 'NextPlot','replacechildren', 'LineWidth', 2)