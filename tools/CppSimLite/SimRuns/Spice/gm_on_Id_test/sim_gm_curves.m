w_nmos = (1:5)*1e-6;
hspc_filename = sprintf('test.hspc');

for i = 1:length(w_nmos)
     hspc_set_param('w_nmos',w_nmos(i),hspc_filename);
     % when adding lines, must always start with 'hspc_addline'
     % followed by 'hspc_addline_continued' statements
     hspc_addline('.param new_param = 1',hspc_filename);
     hspc_addline_continued('.param new_param2 = 10',hspc_filename);
     hspc_addline_continued('.param new_param3 = 15',hspc_filename);
     ngsim(hspc_filename);
     x = loadsig('simrun.raw');
     if i == 1
       current = evalsig(x,'CURRENT');
     end
     m1_gm(i,:) = evalsig(x,'m1_gm');
end

plot(current, m1_gm);
