import numpy as np
import torch
import torch.utils.data
import os
from skimage import io, color
from skimage.transform import rescale
import numpy as np
from tensorboard_logger import configure, log_value


configure("runs/THE3SON", flush_secs=10)



def read_image(filename):
    img = io.imread(filename)
    if len(img.shape) == 2:
        img = np.stack([img, img, img], 2)
    return img

def cvt2Lab(image):
    Lab = color.rgb2lab(image)
    return Lab[:, :, 0], Lab[:, :, 1:]  # L, ab


def cvt2rgb(image):
    return color.lab2rgb(image)


def upsample(image):
    return rescale(image, 4, mode='constant', order=3)
	
	

trainPath = "train.txt"
trainFolder = "gray"
trainGtFolder = "color_64"


trainVectors = []
trainGtVectors = []

try:
	trainVectors = np.load("train_vectors.npy")
except IOError:
	with open(trainPath) as file:
		for line in file:
			rgbimg = read_image(trainFolder + "/" + line.strip())
			labimg = cvt2Lab(rgbimg)
			trainVectors.append([labimg[0]]) # 256*256 each one
			print("Getting train vectors from: " + line)
	np.save("train_vectors.npy", trainVectors)


try:
	trainGtVectors = np.load("train_gtvectors.npy")
except IOError:
	with open(trainPath) as file:
		for line in file:
			gt_rgbimg = read_image(trainGtFolder + "/" + line.strip())
			gt_labimg = cvt2Lab(gt_rgbimg)
			trainGtVectors.append([gt_labimg[1][:,:,0], gt_labimg[1][:,:,1]]) # 64*64*2 each one 
			print("Getting train gt vectors from: " + line)
	np.save("train_gtvectors.npy", trainGtVectors)

print(trainVectors.shape, trainGtVectors.shape)
	
	

valPath = "valid.txt"
valFolder = "gray"
valGtFolder = "color_64"

valVectors = []
valGtVectors = []

try:
    valVectors = np.load("val_vectors.npy")
except IOError:
	with open(valPath) as file:
		for line in file:
			rgbimg = read_image(valFolder + "/" + line.strip())
			labimg = cvt2Lab(rgbimg)
			valVectors.append([labimg[0]])
			print("Gettvng val vectors from: " + line)
	np.save("val_vectors.npy", valVectors)

try:
	valGtVectors = np.load("val_gtvectors.npy")
except IOError:
	with open(valPath) as file:
		for line in file:
			gt_rgbimg = read_image(valGtFolder + "/" + line.strip())
			gt_labimg = cvt2Lab(gt_rgbimg)
			valGtVectors.append([gt_labimg[1][:,:,0], gt_labimg[1][:,:,1]])
			print("getting val gt vectors from " + line)
	np.save("val_gtvectors.npy", valGtVectors)


torch.manual_seed(230)
device = torch.device('cpu')




train = list(zip(trainVectors, trainGtVectors)) #edit
train_loader = torch.utils.data.DataLoader(train, batch_size = 10)

val = list(zip(valVectors, valGtVectors)) #edit
val_loader = torch.utils.data.DataLoader(val, batch_size = 10)

total_step = len(train_loader)
total_val_step = len(val_loader)



model = torch.nn.Sequential(
		torch.nn.Conv2d(1, 16, 5, padding=2),
		torch.nn.MaxPool2d(2, stride=2),
		torch.nn.BatchNorm2d(16),
		torch.nn.ReLU(),
		torch.nn.Conv2d(16, 24, 3, padding=1),
		torch.nn.MaxPool2d(2, stride=2),
		torch.nn.BatchNorm2d(24),
		torch.nn.ReLU(),
		torch.nn.Conv2d(24, 2, 3, padding=1)
        ).double().to(device)

loss_fn = torch.nn.MSELoss(size_average=False)
learning_rate = 1e-3
optimizer = torch.optim.RMSprop(model.parameters(), lr=learning_rate)


total_step = len(train_loader)
epoch = 0





saved_files = os.listdir(os.getcwd() + "/checkpoints/")
if(len(saved_files) > 0):
	saved_files = list(map(int, saved_files))
	last_checkpoint = max(saved_files)
	state = torch.load(os.getcwd() + "/checkpoints/" + str(last_checkpoint))
	model.load_state_dict(state['state_dict'])
	optimizer.load_state_dict(state['optimizer'])
	epoch = state['epoch']
	

while(True):
	epoch += 1
	model.train()
	train_losses = []
	for i, (l,ab) in enumerate((train_loader)):
		x = l
		y = ab
	
		y_pred = model(x)
		loss = loss_fn(y_pred, y)
		train_losses.insert(i, loss)
		
		optimizer.zero_grad()
		loss.backward()
		optimizer.step()
		print ('TRAIN: Epoch {} Step [{}/{}], Loss: {:.4f}' 
                   .format(epoch, i+1, total_step, loss.item()))
		
		
    
	model.eval()
	val_losses = []
	with torch.no_grad():
		for i, (l,ab) in enumerate(val_loader):
			x = l
			y = ab
			
			y_pred = model(x)
			loss = loss_fn(y_pred, y)
			val_losses.insert(i, loss)
			print ('VAL: Epoch {} Step [{}/{}], Loss: {:.4f}' 
					   .format(epoch, i+1, total_val_step, loss.item()))
				   
				   
	print("AVG TRAIN LOSS FOR THIS EPOCH: {}".format(sum(train_losses)/len(train_losses)))	
	log_value('TRAIN', sum(train_losses)/len(train_losses), epoch)
			   
	print("AVG VAL LOSS FOR THIS EPOCH: {}".format(sum(val_losses)/len(val_losses)))
	log_value('VAL', sum(val_losses)/len(val_losses), epoch)
	
	print('Saving: ' + str(epoch))
	state = {'epoch': epoch, 'state_dict': model.state_dict(), 'optimizer': optimizer.state_dict()}
	torch.save(state, os.getcwd() + '/checkpoints/' + str(epoch))
	torch.save(model, os.getcwd() + '/models/' + str(epoch))
