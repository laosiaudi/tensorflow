sudo rm -f /tmp/*.log
bazel build -c opt //tensorflow/tools/pip_package:build_pip_package

sudo bazel-bin/tensorflow/tools/pip_package/build_pip_package /tmp/tensorflow_pkg
sudo -H pip install --force-reinstall /tmp/tensorflow_pkg/tensorflow-0.12.0rc0-cp27-none-linux_x86_64.whl
#sudo python tensorflow/examples/tutorials/mnist/mnist_with_summaries.py
