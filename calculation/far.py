# -*- coding: utf-8 -*-

import os
import numpy as np
from scipy import signal

folder = r"E:\kawai\20190115\2019.01.15_08.22.18_1.1.1.1.B" 
folder_save = r"E:\kawai\20190115"
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




def f(D_where, D2_where):
    D2=np.loadtxt(D_where, delimiter=",")
           
    return(np.amax(D2))



X = np.empty(1024)

for i in range(1024):
    D_where = os.path.join(folder, "2019.01.15_08.22.18_1.%d.1.1.B.txt"%(i+1))
    D2_where = os.path.join(folder, "2019.01.15_08.22.18_1.%d.1.1.D.txt"%(i+1))
    X[i] = f(D_where, D2_where)
    print("%04.f is checked"%i)
    

np.savetxt(os.path.join(folder_save, "far_%04.f.csv"%i) ,X ,delimiter=',')
    
    