// Halide tutorial lesson 21: Auto-Scheduler

#include "Halide.h"

namespace {

using namespace Halide;

///////////////////////// We will define a generator to auto-schedule.
class AutoScheduled : public Halide::Generator<AutoScheduled> {
public:
    Input<Buffer<float>> opencv_out  {"opencv_out"   , 3};
      
    Output<Buffer<uint8_t>> processed {"processed"  , 3};

    void generate() {   
        // forward
        Func clamped("clamped");
        clamped = BoundaryConditions::repeat_edge(opencv_out);

        Func demosaic           = make_demosaic_interp ( &clamped );
        // Func demosaic           = make_demosaic_nn ( &clamped );
        // Func demosaic           = make_demosaic_subsample ( &clamped );

        // Scale back to 0-255 and represent in 8 bit fixed point
        Func descale            = make_descale      ( &demosaic );

        processed(x,y,c) = descale(x,y,c);
 
    }

    void schedule() {
        //opencv_out.dim(0).set_bounds_estimate(0, 4992); [SD]
        opencv_out.dim(0).set_bounds_estimate(0, 512); 
        //opencv_out.dim(1).set_bounds_estimate(0, 3280); [SD]
        opencv_out.dim(1).set_bounds_estimate(0, 256); 
        opencv_out.dim(2).set_bounds_estimate(0, 3);
 

        processed
            .estimate(c, 0, 3)
            .estimate(x, 0, 512)
            .estimate(y, 0, 256);
    }
private:
        // Declare image handle variables
        Var x, y, c;
        Func make_descale( Func *in_func );
        Func make_demosaic_nn( Func *in_func );
        Func make_demosaic_subsample( Func *in_func );
        Func make_demosaic_interp( Func *in_func );
};

Func AutoScheduled::make_descale( Func *in_func ) {
  Var x, y, c;
  // de-scale from 0-1 range to 0-255 range, and cast to 8 bit 
  Func descale("descale");
    descale(x,y,c) = cast<uint8_t>( min( max( (*in_func)(x,y,c) * 255, 0), 255 ) );
  return descale;
}

Func AutoScheduled::make_demosaic_interp( Func *in_func ) {

  // Interpolation demosaicing
  Var x, y, c;
  Func demosaic_interp("demosaic_interp");

  // G R
  // B G

  demosaic_interp(x,y,c) = 
    select(
      // Top green
      y%2==0 && x%2==0,
         select(
          c==0, ((*in_func)(x-1,y,c) + 
                 (*in_func)(x+1,y,c))/2 , //Red
          c==1, (*in_func)(x,y,c)*2, //Green
                ((*in_func)(x,y-1,c) + 
                 (*in_func)(x,y+1,c))/2 ),//Blue
      // Red
      y%2==0 && x%2==1,
        select(
          c==0, (*in_func)(x,y,c),     //Red
          c==1, ((*in_func)(x+1,y,c) + 
                 (*in_func)(x-1,y,c) +
                 (*in_func)(x,y+1,c) +
                 (*in_func)(x,y-1,c))/2 , //Green
                ((*in_func)(x+1,y-1,c) + 
                 (*in_func)(x-1,y+1,c) +
                 (*in_func)(x+1,y+1,c) +
                 (*in_func)(x-1,y-1,c))/4 ),//Blue
      // Blue
      y%2==1 && x%2==0,
        select(
          c==0, ((*in_func)(x+1,y-1,c) + 
                 (*in_func)(x-1,y+1,c) +
                 (*in_func)(x+1,y+1,c) +
                 (*in_func)(x-1,y-1,c))/4 , //Red
          c==1, ((*in_func)(x+1,y,c) + 
                 (*in_func)(x,y+1,c) +
                 (*in_func)(x-1,y,c) +
                 (*in_func)(x,y+1,c))/2 , //Green
                (*in_func)(x,y,c) ),    //Blue
      // Bottom Green
        select(
          c==0, ((*in_func)(x,y-1,c) + 
                 (*in_func)(x,y+1,c))/2 , //Red
          c==1, (*in_func)(x,y,c)*2, //Green
                ((*in_func)(x-1,y,c) + 
                 (*in_func)(x+1,y,c))/2 )//Blue

    );

  return demosaic_interp;
}

Func AutoScheduled::make_demosaic_subsample( Func *in_func ) {

  Var x, y, c;
  Func demosaic_subsample("demosaic_subsample");

  // G R
  // B G

  demosaic_subsample(x,y,c) = 
    select(
      // Top
      y%2==0 && x%2==0,
        select(
          c==0, (*in_func)(x+1,y,c),  //Red
          c==1, (*in_func)(x,y,c)*2,  //Green
                (*in_func)(x,y+1,c)), //Blue
      // Red
      y%2==0 && x%2==1,
        select(
          c==0, (*in_func)(x,y,c),                       //Red
          c==1, (*in_func)(x,y+1,c)+(*in_func)(x-1,y,c), //Green
                (*in_func)(x-1,y+1,c)),                  //Blue
      // Blue
      y%2==1 && x%2==0,
        select(
          c==0, (*in_func)(x+1,y-1,c),                   //Red
          c==1, (*in_func)(x,y+1,c)+(*in_func)(x-1,y,c), //Green
                (*in_func)(x,y,c)),                      //Blue
      // Bottom green
        select(
          c==0, (*in_func)(x,y-1,c), //Red
          c==1, (*in_func)(x,y,c)*2, //Green
                (*in_func)(x-1,y,c)) //Blue
    );

  return demosaic_subsample;
}

Func AutoScheduled::make_demosaic_nn( Func *in_func ) {

  // Nearest neighbor demosaicing
  Var x, y, c;
  Func demosaic_nn("demosaic_nn");

  // G R
  // B G

  demosaic_nn(x,y,c) = 
    select(
      // Top green
      y%2==0 && x%2==0,
        select(
          c==0, (*in_func)(x+1,y,c),  //Red
          c==1, (*in_func)(x,y,c)*2,  //Green
                (*in_func)(x,y+1,c)), //Blue
      // Red
      y%2==0 && x%2==1,
        select(
          c==0, (*in_func)(x,y,c),     //Red
          c==1, (*in_func)(x-1,y,c)*2, //Green
                (*in_func)(x-1,y+1,c)),//Blue
      // Blue
      y%2==1 && x%2==0,
        select(
          c==0, (*in_func)(x+1,y-1,c), //Red
          c==1, (*in_func)(x-1,y,c)*2, //Green
                (*in_func)(x,y,c)),    //Blue
      // Top green
        select(
          c==0, (*in_func)(x,y-1,c), //Red
          c==1, (*in_func)(x,y,c)*2, //Green
                (*in_func)(x-1,y,c)) //Blue
    );

  return demosaic_nn;
}

} // namespace
HALIDE_REGISTER_GENERATOR(AutoScheduled, auto_schedule_gen_fwd)


