# Train Neural Network using TensorFlow

## Steps
1. Prepare training data. [gcc7](../gcc7/generate_train_data).
2. Collect the JSON generated training data, and put into folder 'test1'
3. Train model using TensorFlow: `./train.py test1`
4. Start TensorBoard: `tensorboard --logdir=./model_output`
5. Monitor training process using TensorBoard: `http://localhost:6006`
