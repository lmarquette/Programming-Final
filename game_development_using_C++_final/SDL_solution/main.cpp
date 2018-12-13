#include <iostream>
#include <assert.h>
#include <time.h>
#include <cstdlib>
#include <math.h>
using namespace std;

//include SDL header
#include "SDL2-2.0.8\include\SDL.h"

//load libraries
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2main.lib")

#pragma comment(linker,"/subsystem:console")

struct City
{
	float x, y, w, h;
};


namespace Simulation
{
	SDL_Renderer *renderer = NULL;
	int screen_width = 800;
	int screen_height = 600;

	unsigned char prev_key_state[256];
	unsigned char *keys = NULL;

	int start_node;
	int end_node;

	int *path;
	int *permutated_path;

	float temperature;
	float cooling_rate;
	float current_solution;
	float permutated_solution;
	float cost_difference;

	const int num_cities = 20;
	City city[num_cities];

	void create_Random_Nodes()
	{
		for (int i = 0; i < num_cities; i++)
		{
			city[i].x = (screen_width / 2) + rand() % 300 + (-300);
			city[i].y = (screen_height / 2) + rand() % 300 + (-300);
			city[i].w = 20;
			city[i].h = 20;
		}

	}

	void retrieve_nodes()
	{
		create_Random_Nodes();
	}

	//calculate distance between cities
	float calculating_distance(int *path, City *city, int num_cities)
	{
		float sum = 0.0;
		for (int i = 0; i < num_cities - 1; i++)
		{
			sum += pow(city[path[i + 1]].y - city[path[i]].y, 2)
				+ pow(city[path[i + 1]].x - city[path[i]].x, 2);
		}
		//cout << "sum distance: " << sum << endl;
		return sum;
	}

	//initialize solution by shuffling array
	void initialize_array()
	{
		cout << endl;
		for (int i = 0; i < num_cities; i++)
		{
			path[i] = i;
		}

		for (int i = 0; i < num_cities; i++)
		{
			swap(path[i], path[rand() % (num_cities)]); 
		}

		//debugging
		cout << endl;
		for (int i = 0; i < num_cities - 1; i++)
		{
			if (path[i] == start_node) swap(path[i], path[0]);
			if (path[i] == end_node) swap(path[i], path[num_cities - 1]);
		}
	}

	void init()
	{
		SDL_Init(SDL_INIT_VIDEO);

		prev_key_state[256];
		keys = (unsigned char*)SDL_GetKeyboardState(NULL);

		SDL_Window *window = SDL_CreateWindow(
			"Fortnite",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			screen_width, screen_height, SDL_WINDOW_SHOWN);

		renderer = SDL_CreateRenderer(window,
			-1, SDL_RENDERER_ACCELERATED);

		/*
		
		YOUR INIT CODE
		
		*/
		path = (int*)malloc(sizeof(int)*num_cities);
		permutated_path = (int*)malloc(sizeof(int)*num_cities);

		temperature = 1.0;
		cooling_rate = .98;
		current_solution = 0;
		permutated_solution = 0;
		cost_difference = 0;

		//hard setting startin g and end indexes in array
		start_node = 6;
		end_node = 4;

		initialize_array();
		//find distance
		current_solution = calculating_distance(path, city, num_cities); //failing
		cout << "init sol: " << current_solution << endl;
	}

	//copy data over to permutated array
	void copy_array(int *a, int *b)
	{
		for (int i = 0; i < num_cities; i++)
		{
			b[i] = a[i];
			cout << b[i] << endl;
		}
		
	}

	//simulated annealing
	void sim_annealing()
	{
		bool run = true;
		
			float tmp = 0;
			//current solution



			copy_array(path, permutated_path);

		
			// slightly modify array by swapping 2 values
			swap(permutated_path[rand() % (num_cities - 1) + 1], permutated_path[rand() % (num_cities - 1) + 1]);

			//permutated solution
			permutated_solution = calculating_distance(permutated_path, city, num_cities);

			cout << "sol" << current_solution << endl;
			cout << "permut: " << permutated_solution << endl;
			if (permutated_solution < current_solution)
			{
				current_solution = permutated_solution;
				copy_array(permutated_path, path);
				cout << "sol " << current_solution << endl;
			}
			else
			{
				double rand_num = (double)rand() / RAND_MAX;
				cout << rand_num << endl;
				cost_difference = (current_solution - permutated_solution) / temperature;
				if (rand_num <= exp(cost_difference))
				{
					current_solution = permutated_solution;
					copy_array(permutated_path, path);
					cout << "sol " << current_solution << endl;
				}
			}

			//cool down simulation
			temperature *= cooling_rate;
		
	}

	void draw_Paths()
	{
		for (int i = 0; i < num_cities - 1; i++)
		{
			//cout << "Paths drawn: " << i << endl;
			SDL_RenderDrawLine(renderer, city[path[i]].x + city[path[i]].w*0.5, city[path[i]].y + city[path[i]].h*0.5,
				city[path[i + 1]].x + city[path[i+1]].w*0.5, city[path[i+1]].y + city[path[i+1]].h*0.5); //i+1 is drawing a ghost point, figure out how to fix
		}
	}

	void update()
	{
		//copy previous key state
		memcpy(prev_key_state, keys, 256);

		//consume all window events, key state gets updated here automatically
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}
		}

		/*
		
		YOUR UPDATE CODE
		
		*/

		Simulation::sim_annealing();
		
		if (temperature < 0.001)
		{
			//simulation is over
			//now what
			getchar();
			retrieve_nodes();
			temperature = 1.0;
			initialize_array();
			current_solution = calculating_distance(path, city, num_cities);
		}
	}

	void draw()
	{
		//set color to white
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		//clear screen with white
		SDL_RenderClear(renderer);
		
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		for (int i = 0; i < num_cities; i++)
		{
			SDL_Rect rect;
			rect.h = city[i].h;
			rect.w = city[i].w;
			rect.x = city[i].x;
			rect.y = city[i].y;
			SDL_RenderFillRect(renderer, &rect);
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		draw_Paths();
		

		//flip buffers
		SDL_RenderPresent(renderer);
	}

}
int main(int argc, char **argv)
{
	srand(time(NULL));

	Simulation::retrieve_nodes();

	Simulation::init();


	
	for(;;)
	{
		Simulation::update();
		
		Simulation::draw();
	}


	return 0;
}