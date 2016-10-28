clc, clear, close all
d = [];
l = {'k-mean', 'max', 'mean'};
minl = [4.880400, 5.200850, 5.327730];
minlj = [2, 5, 3];
mm = inf;
jj = 2;
ii = 1;
ll = 'k-mean';
for i=1:size(l, 2)
    m = [];
    k = {};
    for j=1:6
        m(end+1, :) = load(sprintf('fci_%s_%i.dat', l{i}, j));
        k{end+1} = sprintf('grind %i', j);
        if(m(end, end) < mm) 
            d = m(end, :);
            mm = m(end, end);
            ll = l{i};
            ii = i;
            jj = j;
        end
        if(m(end, end) < minl(i))
            minl(i) = m(end, end);
            minlj(i) = j;
        end
    end
    plot(m', 'LineWidth', 2)
    grid on
    ylim([0, max(max(m))])
    legend(k);
    ylabel('Avg. Moves')
    xlabel('Trials')
    title(sprintf('Comparing the grind effect to FCI `%s` combiner method over maze', l{i}))
    %pause
end

fprintf('Winning of the races is %s with %i grind\n', ll, jj);

k = {};
p = [];
for i=1:size(l, 2)
    fprintf('The winning grind of %s combiner method is %i with value of %f\n', l{i}, minlj(i), minl(i));
    p(end+1, :) = load(sprintf('fci_%s_%i.dat', l{i}, minlj(i)));
    k{end+1} = sprintf('method: [%s] grind: [%i] best: [%f]', l{i}, minlj(i), minl(i));
end
plot(p', 'LineWidth', 2)
grid on
ylim([0, max(max(m))])
legend(k);
ylabel('Avg. Moves')
xlabel('Trials')
title('Comparing the best results of 3 FCI combiner method K-MEAN/MEAN/MAX over maze')
legend(k);
%pause

m = [];
m(end+1, :) = p(1, :);
m(end+1, :) = p(2, :);
m(end+1, :) = p(3, :);
m(end+1, :) = load('sep.dat');
m(end+1, :) = load('il.dat');
plot(m', 'LineWidth', 2)
grid
ylim([0, max(max(m))])
legend(sprintf('FCI K-MEAN (%f)', m(1, end)), sprintf('FCI MAX (%f)', m(2, end)), sprintf('FCI MEAN (%f)', m(3, end)), sprintf('SEP (%f)', m(4, end)), sprintf('IL (%f)', m(5, end)));
ylabel('Avg. Moves')
xlabel('Trials')
title('Comparing the FCI/IL/SEP methods')