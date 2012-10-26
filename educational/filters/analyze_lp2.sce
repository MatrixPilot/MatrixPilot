exec('./cvtCSV.sci');

lp2ay = cvtCSV('lp2ay.csv')

N = length(lp2ay);
tflp2 = fft(lp2ay(:,1), -1);
phase = atan(imag(tflp2),real(tflp2))';

dRng = 1:N/2;
figure(1)
subplot(2,1,1)
f = (0:1/N:0.5-1/N);
sRng = 1:length(f);
plot(f, abs(tflp2(sRng))/32768, f, phase(sRng));
mtlb_axis tight; xgrid;
legend("magnitude", "phase");
xlabel("normalized frequency");
title("lp2 IIR filter");

subplot(2,1,2)
plot(lp2ay);
legend("impulse response");
xgrid;
mtlb_axis tight;
xlabel("sample number");
