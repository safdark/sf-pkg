/**
 * @file 
 * @author Denise Ratasich
 * @date 05.09.2013
 *
 * @brief Configuration for ROS node.
 */

#include "configuration/configuration.h"
#include <Eigen/Core>
#include <string>
#include <stdexcept>

using namespace estimation;
using namespace Eigen;

// private functions
std::string getMethod();

#if METHOD == EXTENDED_KALMAN_FILTER  ||  METHOD == UNSCENTED_KALMAN_FILTER
void f(VectorXd& x, const VectorXd& u);
void h(VectorXd& z, const VectorXd& x);
#endif
#if METHOD == EXTENDED_KALMAN_FILTER
void df(MatrixXd& A, const VectorXd& x, const VectorXd& u);
void dh(MatrixXd& H, const VectorXd& x);
#endif

void initEstimatorFactory(EstimatorFactory& factory)
{
  factory.addParam("method", getMethod());

#ifdef WINDOW_SIZE
  factory.addParam("window-size", WINDOW_SIZE);
#endif
#ifdef WEIGHTING_COEFFICIENTS
  VectorXd wc(VECTOR_SIZE(WEIGHTING_COEFFICIENTS));
  CODE_ASSIGN_VALUES_TO_VECTOR(wc, WEIGHTING_COEFFICIENTS);
  factory.addParam("weighting-coefficients", wc);
#endif

#ifdef STATE_TRANSITION_MODEL
  #if METHOD == KALMAN_FILTER
  MatrixXd stm(MATRIX_ROWS(STATE_TRANSITION_MODEL),
	       MATRIX_COLS(STATE_TRANSITION_MODEL));
  CODE_ASSIGN_VALUES_TO_MATRIX(stm, STATE_TRANSITION_MODEL);
  #elif METHOD == EXTENDED_KALMAN_FILTER  ||  METHOD == UNSCENTED_KALMAN_FILTER
  ExtendedKalmanFilter::func_f stm = f;
  #endif
  factory.addParam("state-transition-model", stm);
#endif
#ifdef PROCESS_NOISE_COVARIANCE
  MatrixXd pnc(MATRIX_ROWS(PROCESS_NOISE_COVARIANCE),
	       MATRIX_COLS(PROCESS_NOISE_COVARIANCE));
  CODE_ASSIGN_VALUES_TO_MATRIX(pnc, PROCESS_NOISE_COVARIANCE);
  factory.addParam("process-noise-covariance", pnc);
#endif
#ifdef OBSERVATION_MODEL
  #if METHOD == KALMAN_FILTER
  MatrixXd om(MATRIX_ROWS(OBSERVATION_MODEL),
	       MATRIX_COLS(OBSERVATION_MODEL));
  CODE_ASSIGN_VALUES_TO_MATRIX(om, OBSERVATION_MODEL);
  #elif METHOD == EXTENDED_KALMAN_FILTER  ||  METHOD == UNSCENTED_KALMAN_FILTER
  ExtendedKalmanFilter::func_h om = h;
  #endif
  factory.addParam("observation-model", om);
#endif
#ifdef MEASUREMENT_NOISE_COVARIANCE
  MatrixXd mnc(MATRIX_ROWS(MEASUREMENT_NOISE_COVARIANCE),
	       MATRIX_COLS(MEASUREMENT_NOISE_COVARIANCE));
  CODE_ASSIGN_VALUES_TO_MATRIX(mnc, MEASUREMENT_NOISE_COVARIANCE);
  factory.addParam("measurement-noise-covariance", mnc);
#endif
#ifdef CONTROL_INPUT_MODEL 
  MatrixXd cim(MATRIX_ROWS(CONTROL_INPUT_MODEL),
	       MATRIX_COLS(CONTROL_INPUT_MODEL));
  CODE_ASSIGN_VALUES_TO_MATRIX(cim, CONTROL_INPUT_MODEL);
  factory.addParam("control-input-model", cim);
#endif
#ifdef CONTROL_INPUT
  VectorXd ci(VECTOR_SIZE(CONTROL_INPUT));
  CODE_ASSIGN_VALUES_TO_VECTOR(ci, CONTROL_INPUT);
  factory.addParam("control-input", ci);
#endif
#ifdef INITIAL_STATE
  VectorXd is(VECTOR_SIZE(INITIAL_STATE));
  CODE_ASSIGN_VALUES_TO_VECTOR(is, INITIAL_STATE);
  factory.addParam("initial-state", is);
#endif
#ifdef INITIAL_ERROR_COVARIANCE
  MatrixXd iec(MATRIX_ROWS(INITIAL_ERROR_COVARIANCE),
	       MATRIX_COLS(INITIAL_ERROR_COVARIANCE));
  CODE_ASSIGN_VALUES_TO_MATRIX(iec, INITIAL_ERROR_COVARIANCE);
  factory.addParam("initial-error-covariance", iec);
#endif

#ifdef STATE_SIZE
  factory.addParam("state-size", STATE_SIZE);
#endif
#ifdef MEASUREMENT_SIZE
  factory.addParam("measurement-size", MEASUREMENT_SIZE);
#endif
#ifdef STATE_TRANSITION_MODEL_JACOBIAN
  ExtendedKalmanFilter::func_df stmj = df;
  factory.addParam("state-transition-model-jacobian", stmj);
#endif
#ifdef OBSERVATION_MODEL_JACOBIAN
  ExtendedKalmanFilter::func_dh omj = dh;
  factory.addParam("observation-model-jacobian", omj);
#endif
}

std::string getMethod()
{
#if METHOD == MOVING_MEDIAN
  return "MovingMedian";
#elif METHOD == MOVING_AVERAGE
  return "MovingAverage";
#elif METHOD == KALMAN_FILTER
  return "KalmanFilter";
#elif METHOD == EXTENDED_KALMAN_FILTER
  return "ExtendedKalmanFilter";
#elif METHOD == UNSCENTED_KALMAN_FILTER
  return "UnscentedKalmanFilter";
#endif
}

#if METHOD == EXTENDED_KALMAN_FILTER  ||  METHOD == UNSCENTED_KALMAN_FILTER
void f(VectorXd& x, const VectorXd& u)
{
  if (x.size() != STATE_SIZE)
    throw std::runtime_error("Applying state transition model failed, state vector has invalid size.");

  // create state vector for result, i.e. the a priori state estimate;
  // copying must be done, because x occurs on the right-hand side,
  // e.g.: x[0] = x[1]; x[1] = x[0]; is different to x_apriori[0] =
  // x[1]; x_apriori[1] = x[0];!!
  VectorXd x_apriori(x.size());

  // assign formulas of the state transition model (from the
  // configuration header) to vector x, i.e. calculate a priori state
  // estimate
  CODE_ASSIGN_FORMULAS_TO_VECTOR(x_apriori, STATE_TRANSITION_MODEL);

  // copy back, x will then represents the a priori state estimate
  x = x_apriori;
}

void h(VectorXd& z, const VectorXd& x)
{
  if (z.size() != MEASUREMENT_SIZE)
    throw std::runtime_error("Applying observation model failed, measurement vector has invalid size.");

  // z doesn't occur on the right-hand side, so no copying necessary
  CODE_ASSIGN_FORMULAS_TO_VECTOR(z, OBSERVATION_MODEL);
}
#endif

#if METHOD == EXTENDED_KALMAN_FILTER
void df(MatrixXd& A, const VectorXd& x, const VectorXd& u)
{
  CODE_ASSIGN_FORMULAS_TO_MATRIX(A, STATE_TRANSITION_MODEL_JACOBIAN);
}

void dh(MatrixXd& H, const VectorXd& x)
{
  CODE_ASSIGN_FORMULAS_TO_MATRIX(H, OBSERVATION_MODEL_JACOBIAN);
}
#endif
