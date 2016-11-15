function o = g_const_one(f1, f2)
    assert(all(size(f1) == size(f2)), 'Size mis-match!');
    o = ones(size(f1));
