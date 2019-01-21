# -*- coding: utf-8 -*-

import os
import numpy as np
from scipy.fftpack import fft
from scipy import signal

folder = r"E:\kawai\20181129\20181213\20181213" 
folder_save = r"E:\kawai\20181217"
sampling = 500000 #サンプリング周波数
stage = 3 #ステージの速度(cm/s)
arm = 1 #動かすアームの数

window = (sampling // (stage * arm ))* 4
    
#前処理

def get_N(D_where,D2_where):
    D2=np.loadtxt(D_where, delimiter=",")
    D=np.loadtxt(D2_where, delimiter=",")
       
   
    #centerburst検索
    centerb = np.argmax(D2)
    
    
    #時間窓制限
    d = signal.detrend(D2[centerb - window//2:centerb + window//2])
    d2 = signal.detrend(D[centerb - window//2:centerb + window//2])
    
    
    #zero_crossing
    d2m = d2.mean()
    dstd = np.std(d[0:100000])
    coeff_dstd = 5
    d_c = []
    
    i = 0
    while i < window-1:
        if (d2[i] - d2m) * (d2[i+1] - d2m) < 0 :
            v_comp = d[i] + ((d2[i] - d2m)/(d2[i] - d2[i+1])) * (d[i+1] - d[i])
            if ((i > 332000) and (i < 335000)) or ((v_comp < dstd * coeff_dstd) and (v_comp > (-1)*dstd*coeff_dstd)):
                d_c.append(v_comp)
            else:
                d_c.append(0)
        i += 1
    
    d_c = d_c[:(len(d_c)//2)*2]
    
    N = len(d_c)
    return(N) 




def f(D_where, D2_where, N):
    D2=np.loadtxt(D_where, delimiter=",")
    D=np.loadtxt(D2_where, delimiter=",")
           
    
    #centerburst検索
    centerb = np.argmax(D2)
    
    
    #時間窓制限
    d = signal.detrend(D2[centerb - window//2:centerb + window//2])
    d2 = signal.detrend(D[centerb - window//2:centerb + window//2])
    
    
    #zero_crossing
    d2m = d2.mean()
    dstd = np.std(d[0:100000])
    coeff_dstd = 5
    d_c = []
    
    i = 0
    while i < window-1:
        if (d2[i] - d2m) * (d2[i+1] - d2m) < 0 :
            v_comp = d[i] + ((d2[i] - d2m)/(d2[i] - d2[i+1])) * (d[i+1] - d[i])
            if ((i > 332000) and (i < 335000)) or ((v_comp < dstd * coeff_dstd) and (v_comp > (-1)*dstd*coeff_dstd)):
                d_c.append(v_comp)
            else:
                d_c.append(0)
        i += 1
    
    d_c = d_c[:N]
    
    
    #apodize設定
    apodize_function= np.empty(N)
    apodize_function[:N//2]=np.linspace(0,1,N//2)
    apodize_function[N//2:]=np.linspace(1,0,N//2)
    
    
    #apodizeありでfft(fillingはなし)
    d_c_processed = d_c*apodize_function
    #d_c_processed = np.insert(d_c_processed, 0, np.zeros(500000))
    #d_c_processed = np.append(d_c_processed, np.zeros(500000))
    X=fft(d_c_processed)
    
    return(np.abs(X))



N_min = 1000000
for i in range(101):
    D_where = os.path.join(folder, "%03.f_0.txt"%i)
    D2_where = os.path.join(folder, "%03.f_1.txt"%i)
    N_tmp = get_N(D_where,D2_where)
    if N_tmp < N_min:
        N_min = N_tmp
    print("N is checked in", i)

for i in range(101):
    D_where = os.path.join(folder, "%03.f_0.txt"%i)
    D2_where = os.path.join(folder, "%03.f_1.txt"%i)
    X = f(D_where, D2_where, N_min)
    samplef = 2 * 31606 * arm * stage
    freq = [0]
    for n in range(N_min) :
        if n != 0 and n < N_min/2:
            freq.append((n/((N_min/samplef) * 2 * arm * stage * 1.000275))*0.02998 )
        elif n != 0 :
            freq.append((N_min-n/((N_min/samplef) * 2 * arm * stage * 1.000275))*0.02998  )
    
    freq = np.array(freq)
    data = np.stack((freq[(194.5<freq) & (freq<195)], X[(194.5<freq) & (freq<195)]),axis=1) 
    np.savetxt(os.path.join(folder_save, "%03.f.csv"%i) ,data,delimiter=',')
    print("%03.f.csv is saved"%i)
    