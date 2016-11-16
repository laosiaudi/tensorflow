bazel build -c opt //tensorflow/tools/pip_package:build_pip_package


bazel-bin/tensorflow/tools/pip_package/build_pip_package /tmp/tensorflow_pkg
sudo -H pip install --upgrade --force-reinstall /tmp/tensorflow_pkg/tensorflow-0.11.0rc2-cp27-cp27mu-linux_x86_64.whl
