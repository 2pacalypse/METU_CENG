import numpy as np
import torch
import os
import sys

args = sys.argv
# /models/1layer/100

device = torch.device('cpu')

val_vectors = np.load('test.npy')
val_x = torch.tensor(val_vectors, device=device)


model_names = os.listdir(os.getcwd() + args[1])
model_names.remove('loss.txt')
model_names.sort(key = int)
for m in model_names:
	model = torch.load(os.getcwd() + args[1] + m)	
	model.eval()
	y1 = model(val_x)
	if not os.path.exists(os.getcwd() + '/estimations' + args[1]):
		os.makedirs(os.getcwd() + '/estimations' + args[1])
	
	np.save(os.getcwd() + '/estimations' + args[1] +  m +'TEST', (y1.data.numpy().reshape(-1)))

for m in model_names:
	if(int(m) % 1000 == 0):
	#print("py evaluate.py "  + os.getcwd() +  "/estimations" + args[1] + m + ' valid_gt.npy')
		os.system("py evaluate.py "  + os.getcwd() +  "/estimations" + args[1] + m + 'TEST.npy valid_gt.npy')