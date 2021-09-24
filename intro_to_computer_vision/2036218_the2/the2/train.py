import numpy as np
import torch
import os

torch.manual_seed(230)


train_vectors = np.load('train.npy')
train_gt = np.load('train_gt.npy')
train_gt = np.reshape(train_gt, (-1,1))



device = torch.device('cpu')


# N is batch size; D_in is input dimension;
# H is hidden dimension; D_out is output dimension.
N, D_in, H, D_out = 5000, 512, 256, 1




x = torch.tensor(train_vectors, device=device)
y = torch.tensor(train_gt, device=device, dtype = torch.float)


model = torch.nn.Sequential(
          torch.nn.Linear(D_in, H),
          torch.nn.ReLU(),
		  #torch.nn.Linear(H, H),
		  #torch.nn.ReLU(),
		  #torch.nn.Linear(H, H),
		  #torch.nn.ReLU(),
          torch.nn.Linear(H, D_out),
        ).to(device)

loss_fn = torch.nn.MSELoss(size_average=False)
learning_rate = 1e-4
optimizer = torch.optim.RMSprop(model.parameters(), lr=learning_rate)

layer = '1layer_'  + str(H) + '_'+ str(learning_rate) + '/'


if not os.path.exists(os.getcwd() + "/checkpoints/" + layer):
	os.makedirs(os.getcwd() + "/checkpoints/" + layer)
	os.makedirs(os.getcwd() + "/models/" + layer)

t = 0
saved_files = os.listdir(os.getcwd() + "/checkpoints/" + layer)
if(len(saved_files) > 0):
	saved_files = list(map(int, saved_files))
	last_checkpoint = max(saved_files)
	state = torch.load(os.getcwd() + "/checkpoints/" + layer + str(last_checkpoint))
	model.load_state_dict(state['state_dict'])
	optimizer.load_state_dict(state['optimizer'])
	t = state['epoch'] + 1 
	



while(True):
	y_pred = model(x)
	loss = loss_fn(y_pred, y)
	print(t, loss.item())
	optimizer.zero_grad()
	loss.backward()
	optimizer.step()
	
	if(t % 200 == 0):
		state = {'epoch': t, 'state_dict': model.state_dict(), 'optimizer': optimizer.state_dict()}
		print('Saving: ' + str(t))
		print(t, loss.item(), file=open(os.getcwd() + '/models/' + layer + "loss.txt", "a"))
		torch.save(state, os.getcwd() + '/checkpoints/' + layer + str(t))
		torch.save(model, os.getcwd() + '/models/' + layer + str(t))
	t += 1