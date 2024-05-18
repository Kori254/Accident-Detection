This repository contains code for training a neural network using TensorFlow. The neural network is designed for classification tasks and utilizes various layers such as Dense, Conv1D, Conv2D, MaxPooling1D, MaxPooling2D, and BatchNormalization. The training process includes setting up the model architecture, compiling the model with specified loss and optimizer, and fitting the model to training and validation datasets.
Usage
Requirements

    TensorFlow
    NumPy
    argparse (for parsing command line arguments)

Command Line Arguments

    --epochs: Number of epochs for training (default: 30)
    --learning_rate: Learning rate for the optimizer (default: 0.0005)
    --ensure_determinism: Flag to ensure deterministic behavior (default: False)
    --batch_size: Batch size for training (default: 32)

Data Preparation

Ensure your training and validation datasets are prepared and loaded into TensorFlow tf.data.Dataset objects. Shuffle the training dataset if ENSURE_DETERMINISM is set to False.
Model Architecture

The neural network model consists of several Dense layers with ReLU activation functions. Regularization is applied using L1 regularization with specified regularization coefficients. The output layer uses a softmax activation function for multi-class classification.
Training

Compile the model using categorical cross-entropy loss and the specified optimizer. Train the model using the fit method with the training dataset. Validation data can be provided to monitor model performance during training.
Additional Configuration

    BatchLoggerCallback: A custom callback to log batch metrics during training.
    disable_per_channel_quantization: Flag to disable per-channel quantization for the model.
