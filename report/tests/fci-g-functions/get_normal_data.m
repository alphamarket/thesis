function [x, f] = get_normal_data()
y = randn(1000,1);
x = -5:0.1:5;

n = hist(y,x);

MEAN = mean(y);
STD = sqrt(mean((y - MEAN).^2));

f = (   1/(STD*sqrt(2*pi))   )  *  exp(-0.5*((x-MEAN)/STD).^2  );
f = f*sum(n)/sum(f);
f = (f - min(f)) / (max(f) - min(f));