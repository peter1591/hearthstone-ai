#include <iostream>
#include <fstream>
#include <vector>

#pragma warning (push)
#pragma warning (disable: 4083 4244 4267)
#include "tiny_dnn/tiny_dnn.h"
#pragma warning (pop)

int main(void)
{
	tiny_dnn::network<tiny_dnn::sequential> net;

	net << tiny_dnn::layers::fc(2, 3) << tiny_dnn::activation::tanh()
		<< tiny_dnn::layers::fc(3, 1) << tiny_dnn::activation::tanh();

	std::vector<tiny_dnn::vec_t> input_data{ {-1,-1},{-1,1},{1,-1},{1,1} };
	std::vector<tiny_dnn::vec_t> desired_out{ {-1},{1},{1},{-1} };

	size_t batch_size = 1;
	size_t epoch = 100000;
	tiny_dnn::gradient_descent opt;

	net.fit<tiny_dnn::mse>(opt, input_data, desired_out, batch_size, epoch);

	double loss = net.get_loss<tiny_dnn::mse>(input_data, desired_out);
	std::cout << "mse: " << loss << std::endl;

	for (auto const& data : input_data) {
		auto ret = net.predict(std::vector<tiny_dnn::vec_t>{data}).at(0);
		std::cout << "input: " << data[0] << ", " << data[1] << std::endl;
		std::cout << "output: " << ret.at(0) << std::endl;
		std::cout << std::endl;
	}

	net.save("xor_net");

	std::ofstream ofs("net_example.txt");
	tiny_dnn::graph_visualizer viz(net, "grpah");
	viz.generate(ofs);
	ofs.flush();

	return 0;
}