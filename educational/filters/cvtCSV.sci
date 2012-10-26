function [data] = cvtCSV(fileName)
  
  fd = mopen(fileName,'r');
  txt = mgetl(fd);  // read entire file into RAM
  mclose(fd);
  
  // count non-comment lines
  n = 0;
  for line=txt'
    if (part(line,1) ~= '#') 
      n = n+1;
    end,
  end
  
  // count columns and construct format string
  for line=txt'
    if (part(line,1) ~= '#') 
      commas = strindex(line, ',');
      break;
    end
  end
  nCommas = size(commas,2);
  fmtStr = '%i'; 
  for i=1:nCommas
    fmtStr = strcat([fmtStr ',%i']); 
  end;
  
  data = zeros(n, nCommas+1);
  m = 1;
  for line=txt'
    if (part(line,1) ~= '#') 
      data(m,:) = msscanf(line, fmtStr);
      m = m+1;
    end
  end

endfunction