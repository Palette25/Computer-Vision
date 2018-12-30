import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
import os
from PIL import Image

def InitTestModel():
	writer = tf.python_io.TFRecordWriter("../output/tfResult/testNumbers.tfrecords")
	# Read number from 0 to 9
	for i in range(10):
		targetDir = "../trainData/" + str(i) + "/";
		for imagePath in os.listdir(targetDir):
			path = targetDir + imagePath
			image = Image.open(path)
			image = image.resize((28, 28))
			# Transform into raw image
			rawImg = image.tobytes()
			record = tf.train.Example(features=tf.train.Features(feature={
				'label' : tf.train.Feature(int64_list = tf.train.Int64List(value = [i])),
				'img_raw' : tf.train.Feature(bytes_list = tf.train.BytesList(value = [rawImg]))
			}))
			writer.write(record.SerializeToString())
	writer.close()

# Offical basic methods
def weight_variable(shape, name):
    initial = tf.truncated_normal(shape,stddev = 0.1)
    return tf.Variable(initial, name)

def bias_variable(shape, name):
    initial = tf.constant(0.1,shape = shape)
    return tf.Variable(initial, name)

def conv2d(x,W):
    return tf.nn.conv2d(x, W, strides = [1,1,1,1], padding = 'SAME')

def max_pool_2x2(x):
    return tf.nn.max_pool(x, ksize=[1,2,2,1], strides=[1,2,2,1], padding='SAME')

# Perform testDataset prediction and calculate accuracy
def StartTest():
	# Officially make basic weight and bias variables
	x = tf.placeholder(tf.float32, [None, 784])

	w_conv1 = weight_variable([5,5,1,32], name = 'W1')
	b_conv1 = bias_variable([32],  name = 'B1')

	x_image = tf.reshape(x,[-1, 28, 28, 1])

	h_conv1 = tf.nn.relu(conv2d(x_image,w_conv1) + b_conv1)
	h_pool1 = max_pool_2x2(h_conv1)

	w_conv2 = weight_variable([5, 5, 32, 64],  name = 'W2')
	b_conv2 = bias_variable([64],  name = 'B2')

	h_conv2 = tf.nn.relu(conv2d(h_pool1, w_conv2) + b_conv2)
	h_pool2 = max_pool_2x2(h_conv2)

	w_fc1 = weight_variable([7 * 7 * 64, 1024],  name = 'W3')
	b_fc1 = bias_variable([1024],  name = 'B3')

	h_pool2_flat = tf.reshape(h_pool2, [-1, 7 * 7 * 64])
	h_fc1 = tf.nn.relu(tf.matmul(h_pool2_flat, w_fc1) + b_fc1)

	keep_prob = tf.placeholder("float")
	h_fc1_drop = tf.nn.dropout(h_fc1, keep_prob)

	w_fc2 = weight_variable([1024, 10], name = 'W4')
	b_fc2 = bias_variable([10], name = 'B4')

	y_conv = tf.nn.softmax(tf.matmul(h_fc1_drop, w_fc2) + b_fc2)

	variables_dict = {
		'W1': w_conv1, 'B1': b_conv1, 'W2': w_conv2, 'B2': b_conv2, 'W3': w_fc1, 'B3': b_fc1, 'W4': w_fc2, 'B4': b_fc2
	}
	saver = tf.train.Saver(variables_dict)

	# Read in basic init records
	recordPath = "../output/tfResult/testNumbers.tfrecords"
	fileQueue = tf.train.string_input_producer([recordPath], )
	# Read serialized examples
	reader = tf.TFRecordReader()
	_, examples = reader.read(fileQueue)
	# Parse Features
	features = tf.parse_single_example(examples, features={
		'label' : tf.FixedLenFeature([], tf.int64),
		'img_raw' : tf.FixedLenFeature([], tf.string)
	})
	# Get result in generated streams
	rawImg = tf.decode_raw(features['img_raw'], tf.uint8)
	image = tf.reshape(rawImg, [28, 28, 3])
	label = tf.cast(features['label'], tf.int32)
	# Start session to predict number images and output accuracy
	with tf.Session() as session:
		session.run(tf.global_variables_initializer())
		coord = tf.train.Coordinator()
		queue = tf.train.start_queue_runners(coord=coord)
		saver.restore(session, "../output/tfResult/model.ckpt")
		# Loop to predict
		TestArray = [576, 870, 478, 477, 404, 350, 293, 220, 272, 364]
		totalNum = 0
		correctNum = 0
		for i in range(10):
			for j in range(TestArray[i]):
				img, l = session.run([image, label])
				res = Image.fromarray(img, 'RGB')
				result = res.convert('L')
				# Get predict result
				tv = list(result.getdata())
				tva = [(255 - x) * 1.0 / 255.0 for x in tv]
				pre = tf.argmax(y_conv, 1)
				prediction = pre.eval(feed_dict={x : [tva], keep_prob : 1.0}, session=session)
				if prediction[0] == i :
					correctNum += 1
				totalNum += 1
			print("Finish Number %d, current accuracy: %g" % (i, 1.0 * correctNum / totalNum))
		coord.request_stop()
		coord.join(queue)

if __name__ == '__main__':
	InitTestModel()
	StartTest()