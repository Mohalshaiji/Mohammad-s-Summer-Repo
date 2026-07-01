/*
*	Program: This program is meant to be the prelimenary sketch of the spires simulation
*	of the first experiment of my reservoir Cochlear Implant Front-end project. 
*	It conducts the initial trial in generating neuragrams given an input wav file. 
*	
*	It is assumed that the preprocessing will be done outside of this file to simplify the first pass, 
*	it can be assumed that this will be changed.
*
*	Author: Mohammad Alshaiji < alshaiji@pdx.edu > 
*
*/

#include <spires.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
 
#define N_NEURONS    1000
#define N_CF         40
#define DT           0.25
 
extern double *wav_train, *wav_test;
extern double *neurogram_train, *neurogram_test;
extern int T_train, T_test;
 
int main(void) {
    struct spires_opt_budget budgets[] = {
        { .data_fraction = 0.2, .num_seeds = 2, .time_limit_sec = 60.0   },
        { .data_fraction = 0.5, .num_seeds = 3, .time_limit_sec = 300.0  },
        { .data_fraction = 1.0, .num_seeds = 5, .time_limit_sec = 1200.0 },
    };
 
    struct spires_opt_score score = {
        .lambda_var  = 0.5,
        .lambda_cost = 0.0,
        .metric      = SPIRES_METRIC_AUROC,
    };

    /* ---- Neuron Hyperparameters ---- */
    double fractional_neuron_params[] = {
        1.0,            // V_th
        0.0,            // V_reset
        0.0,            // V_rest
        20.0,           // tau_m
        1.0,            // alpha
        (double)timesteps, // Tmem
        0.1             // bias
    };
    
 
    spires_reservoir_config base_lif = {
        .num_neurons       = N_NEURONS,
        .num_inputs        = 1,
        .num_outputs       = N_CF,
        .spectral_radius   = 0.95,
        .ei_ratio          = 0.8,
        .input_strength    = 0.1,
        .connectivity      = 0.1,
        .dt                = DT,
        .connectivity_type = SPIRES_CONN_RANDOM,
        .neuron_type       = SPIRES_NEURON_LIF_DISCRETE,
        .neuron_params     = NULL,
    };
    spires_reservoir_config base_flif = {
        .num_neurons       = N_NEURONS,
        .num_inputs        = 1,
        .num_outputs       = N_CF,
        .spectral_radius   = 0.95,
        .ei_ratio          = 0.8,
        .input_strength    = 0.1,
        .connectivity      = 0.1,
        .dt                = DT,
        .connectivity_type = SPIRES_CONN_RANDOM,
        .neuron_type       = SPIRES_NEURON_FLIF_GL,
        .neuron_params     = fractional_neuron_params,
    };


    /* AGILE Optimization */
    struct spires_opt_result flif_result;
    if (spires_optimize(&base_flif, budgets, 3, &score, &flif_result,
                        wav_train, neurogram_train, T_train) != 0) return 1;
 
    base.neuron_type = SPIRES_NEURON_LIF_DISCRETE;
    struct spires_opt_result lif_result;
    if (spires_optimize(&base_lif, budgets, 3, &score, &lif_result,
                        wav_train, neurogram_train, T_train) != 0) return 1;
 
    /* train and run FLIF winner */
    spires_reservoir *flif_r = NULL;
    double flif_lambda = pow(10.0, flif_result.best_log10_ridge);
    if (spires_reservoir_create(&flif_result.best_config, &flif_r) != SPIRES_OK) return 1;
    if (spires_train_ridge(flif_r, wav_train, neurogram_train, T_train, flif_lambda) != SPIRES_OK) {
        spires_reservoir_destroy(flif_r); return 1;
    }
    spires_reservoir_reset(flif_r);
    double *pred_flif = spires_run(flif_r, wav_test, T_test);
    spires_reservoir_destroy(flif_r);
    if (!pred_flif) return 1;
 
    /* train and run LIF winner */
    spires_reservoir *lif_r = NULL;
    double lif_lambda = pow(10.0, lif_result.best_log10_ridge);
    if (spires_reservoir_create(&lif_result.best_config, &lif_r) != SPIRES_OK) {
        free(pred_flif); return 1;
    }
    if (spires_train_ridge(lif_r, wav_train, neurogram_train, T_train, lif_lambda) != SPIRES_OK) {
        spires_reservoir_destroy(lif_r); free(pred_flif); return 1;
    }
    spires_reservoir_reset(lif_r);
    double *pred_lif = spires_run(lif_r, wav_test, T_test);
    spires_reservoir_destroy(lif_r);
    if (!pred_lif) { free(pred_flif); return 1; }
 

    /* To-Do: the evaluation section is to go here. the intent is to do a comparison between the lif and flif implementations
	      Further work is required in writing out the evaluation and xcorrelation functions that will be applied 
	      here.
   */
 
    free(pred_flif);
    free(pred_lif);
    return 0;
}
/*
*	Program: This program is meant to be the prelimenary sketch of the spires simulation
*	of the first experiment of my reservoir Cochlear Implant Front-end project. 
*	It conducts the initial trial in generating neuragrams given an input wav file. 
*	
*	It is assumed that the preprocessing will be done outside of this file to simplify the first pass, 
*	it can be assumed that this will be changed.
*
*	Author: Mohammad Alshaiji < alshaiji@pdx.edu > 
*
*/

#include <spires.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
 
#define N_NEURONS    1000
#define N_CF         40
#define DT           0.25
 
extern double *wav_train, *wav_test;
extern double *neurogram_train, *neurogram_test;
extern int T_train, T_test;
 
int main(void) {
    struct spires_opt_budget budgets[] = {
        { .data_fraction = 0.2, .num_seeds = 2, .time_limit_sec = 60.0   },
        { .data_fraction = 0.5, .num_seeds = 3, .time_limit_sec = 300.0  },
        { .data_fraction = 1.0, .num_seeds = 5, .time_limit_sec = 1200.0 },
    };
 
    struct spires_opt_score score = {
        .lambda_var  = 0.5,
        .lambda_cost = 0.0,
        .metric      = SPIRES_METRIC_AUROC,
    };
 
    spires_reservoir_config base_lif = {
        .num_neurons       = N_NEURONS,
        .num_inputs        = 1,
        .num_outputs       = N_CF,
        .spectral_radius   = 0.95,
        .ei_ratio          = 0.8,
        .input_strength    = 0.1,
        .connectivity      = 0.1,
        .dt                = DT,
        .connectivity_type = SPIRES_CONN_RANDOM,
        .neuron_type       = SPIRES_NEURON_LIF_DISCRETE,
        .neuron_params     = NULL,
    };
    spires_reservoir_config base_flif = {
        .num_neurons       = N_NEURONS,
        .num_inputs        = 1,
        .num_outputs       = N_CF,
        .spectral_radius   = 0.95,
        .ei_ratio          = 0.8,
        .input_strength    = 0.1,
        .connectivity      = 0.1,
        .dt                = DT,
        .connectivity_type = SPIRES_CONN_RANDOM,
        .neuron_type       = SPIRES_NEURON_FLIF_GL,
        .neuron_params     = NULL,
    };


    /* AGILE Optimization */
    struct spires_opt_result flif_result;
    if (spires_optimize(&base_flif, budgets, 3, &score, &flif_result,
                        wav_train, neurogram_train, T_train) != 0) return 1;
 
    base.neuron_type = SPIRES_NEURON_LIF_DISCRETE;
    struct spires_opt_result lif_result;
    if (spires_optimize(&base_lif, budgets, 3, &score, &lif_result,
                        wav_train, neurogram_train, T_train) != 0) return 1;
 
    /* train and run FLIF winner */
    spires_reservoir *flif_r = NULL;
    double flif_lambda = pow(10.0, flif_result.best_log10_ridge);
    if (spires_reservoir_create(&flif_result.best_config, &flif_r) != SPIRES_OK) return 1;
    if (spires_train_ridge(flif_r, wav_train, neurogram_train, T_train, flif_lambda) != SPIRES_OK) {
        spires_reservoir_destroy(flif_r); return 1;
    }
    spires_reservoir_reset(flif_r);
    double *pred_flif = spires_run(flif_r, wav_test, T_test);
    spires_reservoir_destroy(flif_r);
    if (!pred_flif) return 1;
 
    /* train and run LIF winner */
    spires_reservoir *lif_r = NULL;
    double lif_lambda = pow(10.0, lif_result.best_log10_ridge);
    if (spires_reservoir_create(&lif_result.best_config, &lif_r) != SPIRES_OK) {
        free(pred_flif); return 1;
    }
    if (spires_train_ridge(lif_r, wav_train, neurogram_train, T_train, lif_lambda) != SPIRES_OK) {
        spires_reservoir_destroy(lif_r); free(pred_flif); return 1;
    }
    spires_reservoir_reset(lif_r);
    double *pred_lif = spires_run(lif_r, wav_test, T_test);
    spires_reservoir_destroy(lif_r);
    if (!pred_lif) { free(pred_flif); return 1; }
 

    /* To-Do: the evaluation section is to go here. the intent is to do a comparison between the lif and flif implementations
	      Further work is required in writing out the evaluation and xcorrelation functions that will be applied 
	      here.
   */
 
    free(pred_flif);
    free(pred_lif);
    return 0;
}
