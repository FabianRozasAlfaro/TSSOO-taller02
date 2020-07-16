#include <checkArgs.hpp>
#include <global.h>
// se siguió con ayuda de los archivos propuestos en U2

uint64_t* array = nullptr; 
uint32_t sumaArray = 0;
std::vector<std::thread *> hilos;
std::vector<std::thread *> hiloSuma;

void fillArray(size_t beginIdx, size_t endIdx, size_t limInfer, size_t limSuper){
	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_int_distribution<> unif(limInfer,limSuper);

	for(size_t i = beginIdx; i < endIdx; i++){
		array[i] = unif(rng);
	}
}


void sumaArrayP(uint32_t &sumaArray, uint32_t beginIdx, uint32_t endIdx){
	sumaArray=0;
	for(uint32_t i = beginIdx; i < endIdx; i++){
		sumaArray += array[i];
	}
	
}
int main(int argc, char** argv){
	uint64_t totalElementos;
	uint32_t numThreads;
	uint32_t limInfer;
	uint32_t limSuper;

	auto argumentos=(std::shared_ptr<checkArgs>) new checkArgs(argc, argv);

	totalElementos = argumentos->getArgs().tamProblema;
	numThreads = argumentos->getArgs().numThreads;
	limInfer = argumentos->getArgs().limInfer;
	limSuper = argumentos->getArgs().limSuper;

	std::cout<< "Tamaño del array: " << totalElementos << std::endl;
	std::cout<< "Threads  : " << numThreads     << std::endl;
	std::cout<< "Limite inferior array: " << limInfer << std::endl;
	std::cout<< "Limite superior array: " << limSuper << std::endl;

	//LLENADO SECUENCIAL
	array = new uint64_t[totalElementos];
	auto start = std::chrono::high_resolution_clock::now();

	fillArray(0, totalElementos, limInfer, limSuper);

	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempo_secuencial = elapsed.count();

	delete[] array;

	//LLENADO EN PARALELO
	array = new uint64_t[totalElementos];
	start= std::chrono::high_resolution_clock::now();
	for(size_t i=0; i < numThreads; i++){
		hilos.push_back(new std::thread(fillArray, i*(totalElementos)/numThreads, (i+1)*(totalElementos)/numThreads, limInfer, limSuper));
	}

	for(auto &thFilled : hilos){
		thFilled->join();
	}

	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
	auto tiempo_paralelo = elapsed.count();

	//PROCESO DE SUMA

	//SUMA SECUENCIAL
	uint32_t suma_s=0;
	start= std::chrono::high_resolution_clock::now();
	sumaArrayP(std::ref(suma_s), 0, totalElementos);
	end = std::chrono::high_resolution_clock::now();
	elapsed= std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoSuma_s = elapsed.count();

	//SUMA PARALELA
	uint32_t suma_p=0;
	start = std::chrono::high_resolution_clock::now();
	for(size_t i=0;i<numThreads;i++){
		hiloSuma.push_back(new std::thread(sumaArrayP, std::ref(suma_p), i*(totalElementos)/numThreads, (i+1)*(totalElementos)/numThreads));
	}
	for(auto &thSuma : hiloSuma){
		thSuma->join();
	}
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoSuma_p = elapsed.count();

	
	// se muestran los resultados, con el tiempo que demoran
	std::cout << "Suma total en paralelo: " << suma_p << std::endl;
	std::cout << "==== Tiempos de las sumas ====" << std::endl;
	std::cout << "Tiempo de la suma secuencial: " << tiempoSuma_s << "[ms]" << std::endl;
	std::cout << "Tiempo de la suma en paralelo: " << tiempoSuma_p << "[ms]" << std::endl;
	std::cout << "SpeedUp : " << (double)tiempoSuma_s/tiempoSuma_p << std::endl;
	std::cout << "==== Tiempos llenado del array ====" << std::endl;
	std::cout << "Tiempo secuencial: " << tiempo_secuencial << "[ms]" << std::endl;
	std::cout << "Tiempo paralelo: " << tiempo_paralelo << "[ms]" << std::endl;
	std::cout << "SpeedUp : " << (double)tiempo_secuencial/tiempo_paralelo << std::endl;
	
	return(EXIT_SUCCESS);
}
