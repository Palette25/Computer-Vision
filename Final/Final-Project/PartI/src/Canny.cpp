#include "Canny.h"

// Use Eight Neighbor to begin DFS Edge points Search
CImg<unsigned char> EdgeDetection(CImg<unsigned char> input, int length){
	CImg<unsigned char> output = CImg<unsigned char>(input.width(), input.height(), 1, 1, 0);
    stack<HoughPos> s;
    queue<HoughPos> q;
    cimg_forXY(input, x, y){
        // If the target point is white, search its 8 neigh different direcvtions
        if(input(x, y) == 255){
            s.push(HoughPos(x, y));
            q.push(HoughPos(x, y));
            input(x ,y) = 0;
            while(!s.empty()){
                HoughPos p = s.top();
                s.pop();
                // search in 8 different direcvtions
                if(p.x - 1 > 0 && p.y - 1 > 0 && input(p.x - 1, p.y - 1) == 255){
                    HoughPos np = HoughPos(p.x - 1, p.y - 1);
                    s.push(np);
                    q.push(np);
                    input(p.x - 1, p.y - 1) = 0;
                }

                if(p.y - 1 > 0 && input(p.x, p.y - 1) == 255){
                    HoughPos np = HoughPos(p.x, p.y - 1);
                    s.push(np);
                    q.push(np);
                    input(p.x, p.y - 1) = 0;
                }

                if(p.x + 1 < input.width() && p.y - 1 > 0 && input(p.x + 1, p.y - 1) == 255){
                    HoughPos np = HoughPos(p.x + 1, p.y - 1);
                    s.push(np);
                    q.push(np);
                    input(p.x + 1, p.y - 1) = 0;
                }

                if(p.x - 1 > 0 && input(p.x - 1, p.y) == 255){
                    HoughPos np = HoughPos(p.x - 1, p.y);
                    s.push(np);
                    q.push(np);
                    input(p.x - 1, p.y) = 0;
                }

                if(p.x + 1 < input.width() && input(p.x + 1, p.y) == 255){
                    HoughPos np = HoughPos(p.x + 1, p.y);
                    s.push(np);
                    q.push(np);
                    input(p.x + 1, p.y) = 0;
                }

                if(p.x - 1 > 0 && p.y + 1 < input.height() && input(p.x - 1, p.y + 1) == 255){
                    HoughPos np = HoughPos(p.x - 1, p.y + 1);
                    s.push(np);
                    q.push(np);
                    input(p.x - 1, p.y + 1) = 0;
                }

                if(p.y + 1 < input.height() && input(p.x, p.y + 1) == 255){
                    HoughPos np = HoughPos(p.x, p.y + 1);
                    s.push(np);
                    q.push(np);
                    input(p.x, p.y + 1) = 0;
                }

                if(p.x + 1 < input.width() && p.y + 1 < input.height() && input(p.x + 1, p.y + 1) == 255){
                    HoughPos np = HoughPos(p.x + 1, p.y + 1);
                    s.push(np);
                    q.push(np);
                    input(p.x + 1, p.y + 1) = 0;
                }
            }

            // No more element in the stack
            if (q.size() > length)
            {
                // The weak edge is not connected to any strong edge. Suppress it.
                while (!q.empty())
                {
                    HoughPos p = q.front();
                    q.pop();
                    output(p.x, p.y) = 255;
                }
            }else{
                // Clean the queue
                while (!q.empty()) q.pop();
            }
        }
    }
    return output;
}