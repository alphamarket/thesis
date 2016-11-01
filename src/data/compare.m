clc, clear, close all

action_pickers = {'boltzmann', 'greedy'};

selected_action_picker = action_pickers{1};

envs_ = {'prey', 'maze'};
methods_ = {'fci-k-mean', 'fci-mean', 'fci-max', 'wsum'};

il_path = 'il';
sep_path = 'sep';
refmat_path = 'refmat';

for e = 1:size(envs_, 2)
    for m = 1:size(methods_, 2)
        path_ = sprintf('%s/method/%s/env/%s/%s', selected_action_picker, refmat_path, envs_{e}, methods_{m});
        ll = dir(sprintf('%s/*.mat', path_));
        data = [];
        legend_ = {};
        for li = 1:size(ll, 1)
            data(end+1, :) = load(sprintf('%s/%s', path_, ll(li).name), '-ascii');
            legend_{end+1} = regexp(ll(li).name, '--refmat-grind \d+', 'match');
            legend_{end} = legend_{end}{1};
        end
        figure('units','normalized','outerposition',[0 0 1 1])
        set(gcf, 'Visible', 'off')
        plot(data', 'LineWidth', 2), grid
        legend(legend_);
        title(sprintf('The grind comparison of `%s` method over %s', methods_{m}, envs_{e}));
        xlabel('Trials')
        ylabel('Avg. Moves');
        savefig(sprintf('%s/%s-%s-grind-compare.fig', path_, envs_{e}, methods_{m}))
        print(sprintf('%s/%s-%s-grind-compare.png', path_, envs_{e}, methods_{m}), '-r300', '-dpng')
    end
end