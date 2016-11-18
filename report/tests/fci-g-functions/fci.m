clc, close all, clear
a = ones(1, 20);
b = ones(1, 20) * 2;
c = ones(1, 20) * 3;
m = [0.1, 0.4, 0.3];

figure('units','normalized','outerposition',[0 0 1 1])
set(gcf, 'Visible', 'off')
m_ = {'-','--','*','x','+','-.','^','v','>','<','p','h'};
set(gca, 'LineStyleOrder',m_, 'NextPlot','replacechildren', 'LineWidth', 2)
grid on, ylim([0, 4]), xlim([1, length(a)]);
set(gca, 'Xtick', 0:20/8:10000)
set(gca, 'Ytick', 0:.5:10000)
set(gca, 'LineWidth', 2);
hold on;

plot(a, 'LineWidth', 2);
plot(b, 'k', 'LineWidth', 2);
plot(c, 'r', 'LineWidth', 2);
% max
plot((a - 0) * 1 + (b - a) * max(m(1:2)) + (c - b) * m(3), '--', 'Color', [0.3010    0.7450    0.9330], 'LineWidth', 2);
% mean
plot((a - 0) * 1 + (b - a) * mean(m(1:2)) + (c - b) * m(3), '<', 'Color', [0.3010    0.7450    0.9330], 'LineWidth', 2);
% k-mean
plot((a - 0) * 1 + (b - a) * (2 * max(m(1:2)) + min(m(1:2))) / 2 + (c - b) * m(3), 'p', 'Color', [0.3010    0.7450    0.9330], 'LineWidth', 2);
% const-one
plot((a - 0) * 1 + (b - a) * 1 + (c - b) * 1, '+', 'Color', [0.3010    0.7450    0.9330], 'LineWidth', 2);

legend('info#1 (g_1 = 0.1)', 'info#2 (g_2 = 0.4)', 'info#3 (g_3 = 0.3)', 'Max', 'Mean', 'K-Mean', 'Const-One')
savefig('fci.fig')
print('fci.png', '-r300', '-dpng')