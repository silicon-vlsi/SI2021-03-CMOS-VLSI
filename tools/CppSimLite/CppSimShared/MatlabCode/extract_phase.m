function [phase,avg_period] = extract_phase(raw_period_seq,raw_period_seq_ref)

count = 0;
for i = 1:length(raw_period_seq)
   if (raw_period_seq(i) ~= 0.0)
     count = count + 1;
   end
end

period_seq = zeros(count,1);

k = 1;
for i = 1:length(raw_period_seq)
   if (raw_period_seq(i) ~= 0.0)
      period_seq(k) = raw_period_seq(i);
      k = k + 1;
   end
end


if (nargin == 2)

   period_seq2 = zeros(count,1);

   k = 1;
   for i = 1:length(raw_period_seq_ref)
      if (raw_period_seq_ref(i) ~= 0.0)
         period_seq2(k) = raw_period_seq_ref(i);
         k = k + 1;
         if (k > count)
           break;
         end
      end
   end

   avg_period = mean(period_seq2);
   phase = filter(1,[1 -1],period_seq-period_seq2)/avg_period;
else
   avg_period = mean(period_seq);
   phase = filter(1,[1 -1],period_seq-avg_period)/avg_period;
end
