function compare()
    clear, clc, close all

    envs_ = {'maze', 'prey'};

    il_path = 'il';
    sep_path = 'sep';
    refmat_path = 'refmat';

    pathes_ = {il_path, refmat_path, sep_path};

    for e_ = 1:size(envs_, 2)
        data = [];
        legend_ = {};
        for p_ = 1:size(pathes_, 2)
            path_ = sprintf('%s/env/%s', pathes_{p_}, envs_{e_});
            ll = dir(sprintf('%s/*.mat', path_));
            for li = 1:size(ll, 1)
                data(end+1, :) = mean(load(sprintf('%s/%s', path_, ll(li).name), '-ascii'));
                legend_{end+1} = regexp(ll(li).name, '--method \w+', 'match');
                legend_{end} = legend_{end}{1};
                if(strcmp(pathes_{p_}, refmat_path))
                    l = regexp(ll(li).name, '--refmat-combinator [a-zA-Z-]+', 'match');
                    legend_{end} = sprintf('%s %s', legend_{end}, l{1});
                end
            end
        end
        open_hidden_fig()
        plot(data', 'LineWidth', 2)
        legend(legend_)
        title(sprintf('The average Q-Table''s values over %s', envs_{e_}));
        xlabel('STATE/ACTION');
        ylabel('Q VALUE');
        grid
        savefig(sprintf('%s-qtable-compare.fig', envs_{e_}))
        print(sprintf('%s-qtable-compare.png', envs_{e_}), '-r300', '-dpng')
    end

function open_hidden_fig()
    figure('units','normalized','outerposition',[0 0 1 1])
    set(gcf, 'Visible', 'off')
    m = {'-','--','*','x','+','-.','^','v','>','<','p','h'};
    set(gca, 'LineStyleOrder',m, 'NextPlot','replacechildren', 'LineWidth', 2)
