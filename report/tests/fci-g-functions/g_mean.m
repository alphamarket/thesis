function o = g_mean(f1, f2)
    assert(all(size(f1) == size(f2)), 'Size mis-match!');
    o = zeros(size(f1));
    for i = 1:length(f1)
        o(i) = mean([f1(i), f2(i)]);
    end