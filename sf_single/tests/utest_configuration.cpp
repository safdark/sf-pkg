#include <gtest/gtest.h>
#include <typeinfo>
#include <iostream>
#include <vector>

// testing following API
#include "Configurator.h"

using namespace std;

// -----------------------------------------
// tests
// -----------------------------------------
TEST(ConfigurationTest, addParams)
{
  Configurator cfg;

  EXPECT_NO_THROW(cfg.addParam("string", string("xyz")));
  EXPECT_NO_THROW(cfg.addParam("int", 5));
  std::vector< double > v = {1, 0, 1, 3, 1};
  EXPECT_NO_THROW(cfg.addParam("vector", v));
  std::vector< std::vector<double> > m = { {1,0}, {0,1} };
  EXPECT_NO_THROW(cfg.addParam("matrix", m));
}

TEST(ConfigurationTest, methodToEstimatorCheck)
{
  Configurator cfg;
  estimation::IEstimationMethod* estimator;
  string classname;

  // no params, invalid
  cfg.addParam("method", string("MovingMedian"));
  cfg.reset();
  EXPECT_ANY_THROW(estimator = cfg.getInitializedEstimator());
  cfg.addParam("method", 5);
  EXPECT_ANY_THROW(estimator = cfg.getInitializedEstimator());

  // add params and create according estimator
  // check a moving median
  cfg.reset();
  cfg.addParam("method", string("MovingMedian"));
  cfg.addParam("window-size", 5);
  EXPECT_NO_THROW(estimator = cfg.getInitializedEstimator());
  classname = typeid(*estimator).name();
  EXPECT_NE(string::npos, classname.find("MovingMedian",0));

  // check a moving average
  cfg.reset();
  cfg.addParam("method", string("MovingAverage"));
  cfg.addParam("window-size", 5);
  std::vector< double > wc1 = {1, 2, 5, 2};
  cfg.addParam("weighting-coefficients", wc1);
  EXPECT_ANY_THROW(estimator = cfg.getInitializedEstimator());

  cfg.reset();
  cfg.addParam("method", string("MovingAverage"));
  cfg.addParam("window-size", 5);
  std::vector< double > wc2 = {1, 2, 5, 2, 1};
  cfg.addParam("weighting-coefficients", wc2);
  EXPECT_NO_THROW(estimator = cfg.getInitializedEstimator());
  classname = typeid(*estimator).name();
  EXPECT_NE(string::npos, classname.find("MovingAverage",0));

  // check a kalman filter
  cfg.reset();
  cfg.addParam("method", string("KalmanFilter"));
  std::vector< std::vector<double> > stm = { {1} };
  cfg.addParam("state-transition-model", stm);
  EXPECT_ANY_THROW(estimator = cfg.getInitializedEstimator());
  std::vector< std::vector<double> > pnc = { {0.2} };
  cfg.addParam("process-noise-covariance", pnc);
  EXPECT_ANY_THROW(estimator = cfg.getInitializedEstimator());
  std::vector< std::vector<double> > om = { {1} };
  cfg.addParam("observation-model", om);
  EXPECT_ANY_THROW(estimator = cfg.getInitializedEstimator());
  std::vector< std::vector<double> > mnc = { {10} };
  cfg.addParam("measurement-noise-covariance", mnc);
  EXPECT_NO_THROW(estimator = cfg.getInitializedEstimator());
  classname = typeid(*estimator).name();
  EXPECT_NE(string::npos, classname.find("KalmanFilter",0));
}

// -----------------------------------------
// Run all tests
// -----------------------------------------
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}