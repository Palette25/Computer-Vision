from tensorflow.examples.tutorials.mnist import input_data
from PIL import Image
import tensorflow as tf
import matplotlib.pyplot as plt
import numpy as np
import os

# Init Basic TFRecord training frames
def initTFRecord():
	trainPath = "../trainData/"
	# Define basic cluster classes
	classes = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}
	# Define record writer
	writer = tf.python_io.TFRecordWriter("../output/tfResult/train.tfrecords")
	# Loop to init basic record
	for index, number in enumerate(classes):
		classPath = trainPath + str(index) + '/'
		for imagePath in os.listdir(classPath):
			path = classPath + imagePath
			image = Image.open(path)
			image = image.resize((28, 28))
			
			# Transform origin image to raw image
			rawImg = image.tobytes()
			record = tf.train.Example(features=tf.train.Features(feature={
				'label' : tf.train.Feature(int64_list = tf.train.Int64List(value = [index])),
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

def decodeTFRecord(filePath):
	# Create queue to reader input file list
	queue = tf.train.string_input_producer([filePath], shuffle = False, num_epochs = 4000 * 100)
	reader = tf.TFRecordReader()
	_, examples = reader.read(queue)
	print(queue.size())
	# Parse readin examples
	features = tf.parse_single_example(examples, features={
		'label' : tf.FixedLenFeature([], tf.int64),
		'img_raw' : tf.FixedLenFeature([], tf.string)
	})
	# Cast into pixel array
	image = tf.decode_raw(features['img_raw'], tf.uint8)
	image = tf.reshape(image, [28, 28, 3])
	label = tf.cast(features['label'], tf.int32)
	return image, label

# Create batches for trained data process
def makeBatch(filePath, batchSize, batchNumber):
	image, label = decodeTFRecord(filePath)
	# Make disorder trained images
	realSize = batchNumber * batchSize
	# Add Dequeue parameter
	deQueuePara = 4200
	realSize = realSize + deQueuePara
	# Create batches
	imageBatch, labelBatch = tf.train.shuffle_batch([image, label], 
		batch_size=batchSize, capacity=realSize, min_after_dequeue=deQueuePara
	)
	# One hot for label batch
	labelBatch = tf.one_hot(labelBatch, depth=10)
	return imageBatch, labelBatch

# Train model with training images
def trainModel():
	# Init placeholder
	x = tf.placeholder(tf.float32, [None, 784])
	y = tf.placeholder(tf.float32, [None, 10])
	# Init CNN Basic nerual variables
	w_conv1 = weight_variable([5, 5, 1, 32], name = "W1")
	b_conv1 = bias_variable([32], name = "B1")

	x_image = tf.reshape(x,[-1,28,28,1])

	h_conv1 = tf.nn.relu(conv2d(x_image,w_conv1) + b_conv1)
	h_pool1 = max_pool_2x2(h_conv1)

	w_conv2 = weight_variable([5, 5, 32, 64], name = "W2")
	b_conv2 = bias_variable([64], name = "B2")

	h_conv2 = tf.nn.relu(conv2d(h_pool1, w_conv2) + b_conv2)
	h_pool2 = max_pool_2x2(h_conv2)

	w_fc1 = weight_variable([7 * 7 * 64, 1024], name="W3")
	b_fc1 = bias_variable([1024], name = "B3")

	h_pool2_flat = tf.reshape(h_pool2, [-1, 7 * 7 * 64])
	h_fc1 = tf.nn.relu(tf.matmul(h_pool2_flat, w_fc1) + b_fc1)

	keep_prob = tf.placeholder("float")
	h_fc1_drop = tf.nn.dropout(h_fc1, keep_prob)

	w_fc2 = weight_variable([1024, 10], name="W4")
	b_fc2 = bias_variable([10],name="B4")

	y_conv=tf.nn.softmax(tf.matmul(h_fc1_drop, w_fc2) + b_fc2)
	# Init define CNN's basic train variables
	cross_entropy = -tf.reduce_sum(y * tf.log(y_conv))
	train_step = tf.train.AdamOptimizer(1e-4).minimize(cross_entropy)
	correct_predicition = tf.equal(tf.argmax(y_conv, 1), tf.argmax(y, 1))
	accuracy = tf.reduce_mean(tf.cast(correct_predicition, "float"))
	variables_dict = {
		'W1': w_conv1, 'B1': b_conv1, 'W2': w_conv2, 'B2': b_conv2, 'W3': w_fc1, 'B3': b_fc1, 'W4': w_fc2, 'B4': b_fc2
	}
	saver = tf.train.Saver(variables_dict)
	# Get batches before training
	trainFile = "../output/tfResult/train.tfrecords"
	imageBatch, labelBatch = makeBatch(filePath=trainFile, batchSize=100, batchNumber=4000)
	# Start training
	with tf.Session() as session:
		# Grouping
		group = tf.group(tf.global_variables_initializer(),tf.local_variables_initializer())
		session.run(group)
		coord = tf.train.Coordinator()
		Queue = tf.train.start_queue_runners(coord=coord)
		for index in range(4000):
			img, l = session.run([imageBatch, labelBatch])
			for i in range(100):
				image = Image.fromarray(img[i], 'RGB')
				image = image.convert('L')

				# Calculting tv and tva
				tv = list(image.getdata())
				tva = [(255 - x) * 1.0 / 255.0 for x in tv]
			# Run train step
			train_step.run(feed_dict={
				x : [tva], y : [l[i]], keep_prob : 0.5
			})
			# Per 100 steps print accuracy
			if index % 100 == 0:
				train_accuracy = accuracy.eval(feed_dict={
					x : [tva], y : [l[i]], keep_prob : 1.0
				})
				print("Training Step %d, accuracy: %f" % (index, train_accuracy))
		coord.request_stop()
		coord.join(Queue)
		saver.save(session, '../output/tfResult/model.ckpt')

if __name__ == '__main__':
	initTFRecord()
	trainModel()