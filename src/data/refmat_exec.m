clear
refmat1 = load('refmat.dat');
refmat2 = load('refmat3.dat');

N = 1000;
B1 = zeros(20, N, 4);
B2 = zeros(20, N, 4);
for i=1:20
    B1(i, :, :) = refmat1(((i-1)*N + 1):(i*N),:);
    B2(i, :, :) = refmat2(((i-1)*N + 1):(i*N),:);
end

refmat1 = squeeze(mean(B1, 1));
refmat2 = squeeze(mean(B2, 1));

for i = 1:4
    subplot(2, 2, i)
    plot(refmat2(:, i), 'LineWidth', 1.2)
    hold
    plot(refmat1(:, i), 'r', 'LineWidth', 1.2)
    xlabel('Trials')
    ylabel('Counts')
    title(sprintf('[%i,%i]', floor((i-1) / 2), floor(mod(i-1, 2))));
    legend('agend 1', 'agent 2')
end
