function plot_data()
    [~, fp] = get_normal_data();
    f1 = circshift(fp, 50, 2);
    f2 = fp ./ 2;
	o1 = g_max(f1, f2);
	o2 = g_mean(f1, f2);
	o3 = g_k_mean(f1, f2);
	o4 = g_const_one(f1, f2);
    m = {'-','--','*','x','+','-.','^','v','>','<','p','h'};
    % figure('units','normalized','outerposition',[0 0 1 1])
    figure, hold on, grid on, ylim([0, 1.25]), xlim([0, length(f1)]);
    set(gca, 'Xtick', 0:5:10000)
    set(gca, 'Ytick', 0:.1:10000)
    set(gca, 'LineStyleOrder',m, 'NextPlot','replacechildren', 'LineWidth', 2);
    a = [f1; f2; o1; o2; o3; o4]';
    c = [
    	0    0.4470    0.7410
	    0.8500    0.3250    0.0980
	    0.9290    0.6940    0.1250
	    0.4940    0.1840    0.5560
	    0.4660    0.6740    0.1880
	    0.3010    0.7450    0.9330
	    0.6350    0.0780    0.1840];
    for i = 1:size(a, 2)
    	hold on
    	plot(a(:, i), m{i}, 'Color', c(i, :), 'LineWidth', 2)
    end
    % plot(, 'LineWidth', 2);
    title('The effects of g(\cdot) functions over a distribution space');
    legend('Distribution# 1', 'Distribution# 2', 'Max', 'Mean', 'K-Mean', 'Const-One');
