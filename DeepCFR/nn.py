# import config variable: n_train, n_batch etc
from config import *

import numpy as np
import tensorflow as tf 
import keras

from keras.layers import Input, Dense, BatchNormalization, Flatten
from keras.activations import relu, softmax
from keras.optimizers import Adam
from keras.models import Model
from keras.layers import add, concatenate


    
# fully connected block relu(Ax[+x])
def FC(x, size, skip_connection = False):
    y = Dense(size)(x)
    if skip_connection == True:
        y = add([x, y])
    
    return relu(y)

# t is iteration number
def custom_loss(y_true__t, y_pred): 
    y_true, t = y_true__t
    loss = t * np.square(y_true - y_pred)
    return np.sum(loss)
    
# mode can be "value" or "policy", specify what network to build
def build(mode):
    #card layer
    card_input = Input(shape=card_input_shape)
    x = Flatten()(card_input)
    x = FC(x, size= 192)
    x = FC(x, size= 192, skip_connection=True)
    card_layer = FC(x, size= 64)
    
    # bet layer
    bet_input = Input(shape=bet_input_shape)
    x = Flatten()(bet_input)
    x = FC(x, size= 64)
    bet_layer = FC(x, size= 64, skip_connection=True)
    
    merge_layer = concatenate([card_layer, bet_layer])
    x = FC(merge_layer, 64)
    x = FC(merge_layer, 64, skip_connection=True)
    x = FC(merge_layer, 64, skip_connection=True)
    x = FC(merge_layer, 3)
    
    x = BatchNormalization()(x)
    
    if (mode == "policy"): # apply softmax
        x = softmax(x)
    
    model = Model(inputs=[card_input, bet_input], outputs=[x])
    model.compile(optimizer=Adam(),
                loss=custom_loss,
                clipnorm=1.)
                
    return model
    
# mode can be "value" or "policy", represent what nn we train.
def train(old_nn, buffer, mode="value"):
    data = []
    label = []
    for I, y, t in buffer:
        data.append(I)
        label.append(np.append(y, t))
    
    data = np.array(data)
    label = np.array(label)
            
    new_nn = build(mode)
    new_nn.fit(data, label, epoch = n_train, batch_size = n_batch)
    return new_nn
    
        
        