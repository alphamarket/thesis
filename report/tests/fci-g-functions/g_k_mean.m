function o = g_k_mean(f1, f2)
    assert(all(size(f1) == size(f2)), 'Size mis-match!');
    o = zeros(size(f1));
    for i = 1:length(f1)
        a = sort([f1(i), f2(i)]);
        for k = 1:length(a)
            o(i) = o(i) + a(k) * k;
        end
        o(i) = o(i) / sum([1:length(a)]);
    end