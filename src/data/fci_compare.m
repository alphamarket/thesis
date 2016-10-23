clear, close all
a = {'IL.dat','fci_mean.dat', 'fci_max.dat', 'fci_k_mean.dat'};
figure, hold
B = [];
for i=1:size(a, 2)
    B(end+1, :) = mean(load(a{i}));
    fprintf('min(%s) : %f\n', a{i}, min(B(end))); 
end
plot(B', 'LineWidth', 2)
set(legend('IL','fci_mean', 'fci_max', 'fci_k_mean'), 'Interpreter', 'none')