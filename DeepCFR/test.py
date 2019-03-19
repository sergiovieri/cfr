import numpy as np
import tensorflow as tf
from tensorflow import keras

from tensorflow.keras.layers import Input, Dense, BatchNormalization, Flatten
from tensorflow.keras.activations import relu
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.models import Model


def custom_loss(y_true, y_pred):
    return (y_pred - y_true) ** 4


input = Input(shape=(2, 2, ))
x = Flatten()(input)
print("print input : " + str(x.shape))
x = Dense(32, activation=relu)(x)
x = Dense(32, activation=relu)(x)
x = Dense(4, activation=relu)(x)
# x = Flatten(1)(x)
print("print input (x) : " + str(x.shape))

model = Model(inputs=[input], outputs=[x])

model.compile(optimizer=Adam(),
              loss=custom_loss
              )


data = np.random.random((1000, 2, 2))
label = np.random.random((1000, 4))

print(label.shape)
# x_test = np.random.random((100, 2))
# y_test = np.array([x[0] + x[1] for x in data]);

model.fit(data, label, epochs = 100, batch_size = 32)


    