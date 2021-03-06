/**
 * @file 
 * @author Denise Ratasich
 * @date 09.09.2013
 *
 * @brief Prepares configuration for the ROS node.
 *
 * Abbreviations or macros respectively for the ROS node are generated
 * here with the Boost preprocessor library.
 */

#ifndef __CONFIGURATION_POSTCONFIG_H__
#define __CONFIGURATION_POSTCONFIG_H__

// boost preprocessor includes
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/for.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/comparison/not_equal.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#include "estimation/InputValue.h"


// ----------------------------------------------------------------
// SIZE defines for ROS node, and abbreviations for checking SIZES
// ----------------------------------------------------------------

#if (defined STATE_TRANSITION_MODEL)  &&  (defined OBSERVATION_MODEL)
  #define STATE_SIZE		BOOST_PP_SEQ_SIZE(STATE_TRANSITION_MODEL)	// n
  #define MEASUREMENT_SIZE	BOOST_PP_SEQ_SIZE(OBSERVATION_MODEL)		// m
  #ifdef TOPICS_IN_CTRL
    #define CONTROL_SIZE		BOOST_PP_SEQ_SIZE(TOPICS_IN_CTRL)	// l
  #endif
#endif

/** 
 * @brief Expands to the number of elements in a sequence.
 */
#define VECTOR_SIZE(vector)	\
  BOOST_PP_SEQ_SIZE(vector)	\
  /**/

/** 
 * @brief Expands to the number of elements in a sequence, i.e. for a
 * sequence-of-sequences its the number of rows.
 */
#define MATRIX_ROWS(matrix)	\
  BOOST_PP_SEQ_SIZE(matrix)	\
  /**/

/** 
 * @brief Expands to the number of elements of the first sequence of a
 * sequence, i.e. for a sequence-of-sequences its the number of
 * columns.
 */
#define MATRIX_COLS(matrix)				\
  BOOST_PP_SEQ_SIZE(BOOST_PP_SEQ_ELEM(0,matrix))	\
  /**/

// ----------------------------------------------------------------
// abbreviations for handling INPUTS/OUTPUTS
// ----------------------------------------------------------------

/** @brief Expands to the number of input topics representing
 * measurements defined in the configuration header. */
#define TOPICS_IN_MEAS_NUM	BOOST_PP_SEQ_SIZE(TOPICS_IN)
/** @brief Expands to the number of input topics representing control
 * input variables defined in the configuration header. */
#ifdef TOPICS_IN_CTRL
  #define TOPICS_IN_CTRL_NUM	BOOST_PP_SEQ_SIZE(TOPICS_IN_CTRL)
#endif

/** @brief Expands to the name of a topic T. */
#define TOPIC_IN_NAME(T)		BOOST_PP_SEQ_ELEM(0, T)
/** @brief Expands to the name of a topic T as string. */
#define TOPIC_IN_NAME_STR(T)		BOOST_PP_STRINGIZE(TOPIC_IN_NAME(T))
/** @brief Expands to the type of a topic T. */
#define TOPIC_IN_TYPE(T)		BOOST_PP_SEQ_ELEM(1, T)
/** @brief Expands to the value to estimate of a topic T. */
#define TOPIC_IN_VALUE(T)		BOOST_PP_SEQ_ELEM(2, T)
/** @brief Expands to the value to estimate of a topic T as string. */
#define TOPIC_IN_VALUE_STR(T)		BOOST_PP_STRINGIZE(TOPIC_IN_VALUE(T))
/** @brief Expands to the variance of a topic T. Use this macro only
 * if you are sure that the variance is given, e.g. use
 * TOPIC_IN_HAS_VARIANCE for checking. */
#define TOPIC_IN_VARIANCE(T)		BOOST_PP_SEQ_ELEM(3, T)
/** @brief Expands to the variance of a topic T as string. Use this
 * macro only if you are sure that the variance is given, e.g. use
 * TOPIC_IN_HAS_VARIANCE for checking. */
#define TOPIC_IN_VARIANCE_STR(T)	BOOST_PP_STRINGIZE(TOPIC_IN_VARIANCE(T))
/** @brief Expands to 1 when an additional field containing the
 * variance is specified, otherwise 0. */
#define TOPIC_IN_HAS_VARIANCE(T)	BOOST_PP_EQUAL(BOOST_PP_SEQ_SIZE(T),4)

/** @brief Expands to the number of output topics defined in the
 * configuration header. */
#define TOPICS_OUT_NUM	BOOST_PP_SEQ_SIZE(TOPICS_OUT)

/** @brief Expands to the name of a topic T. */
#define TOPIC_OUT_NAME(T)	BOOST_PP_SEQ_ELEM(0, T)
/** @brief Expands to the name of a topic T. */
#define TOPIC_OUT_NAME_STR(T)	BOOST_PP_STRINGIZE(TOPIC_OUT_NAME(T))
/** @brief Expands to the field to estimate of a topic T. */
#define TOPIC_OUT_INDEX(T)	BOOST_PP_SEQ_ELEM(1, T)

// ----------------------------------------------------------------
// subscribing to topics
// ----------------------------------------------------------------

/** @brief Initializes an element of the given array of subscribers
 * according to a topic. */
#define SUBSCRIBE_TO_TOPIC(r, data, i, elem)			\
  BOOST_PP_SEQ_ELEM(0, data)[i] =				\
    BOOST_PP_SEQ_ELEM(1, data).subscribe<TOPIC_IN_TYPE(elem)>	\
    (								\
     TOPIC_IN_NAME_STR(elem),					\
     20,							\
     BOOST_PP_CAT(BOOST_PP_SEQ_ELEM(2, data),i)			\
    );								\
  ROS_INFO("Subscribing to '%s'.", TOPIC_IN_NAME_STR(elem));	\
  /**/

/** 
 * @brief Initializes the subscribers.
 *
 * The topics are defined as a sequence of tuples. Each tuple is
 * processed to create a new subscriber which is stored in the given
 * array \c subscribers. 
 */
#define SUBSCRIBE(topics, subscribers, handle, fctPrefix)		\
  BOOST_PP_SEQ_FOR_EACH_I(SUBSCRIBE_TO_TOPIC, (subscribers)(handle)(fctPrefix), topics) \
  /**/

/** 
 * @brief Creates the callback function for a received message of a
 * specific topic.
 *
 * The received message is stored into data (must be of type struct
 * TopicState) for further processing.
 *
 * @param r Unnecessary, needed by Boost for repetitions.
 * @param data Must be an array of struct TopicState (defined in
 * sf_filter.cpp) which saves the important parts of the message
 * needed for estimation.
 * @param i Number of the topic (position in the sequence TOPICS_IN
 * defined by the user).
 * @param elem The topic tuple from TOPICS_IN, i.e. name, type,
 * value[, variance] of message to receive.
 */
#define RECEIVE_TOPIC(r, data, i, elem)					\
  void BOOST_PP_CAT(BOOST_PP_SEQ_ELEM(1, data),i)(			\
    const TOPIC_IN_TYPE(elem)::ConstPtr& msg				\
    )									\
  {									\
    BOOST_PP_SEQ_ELEM(0, data)[i].stamp = ros::Time::now();		\
    BOOST_PP_SEQ_ELEM(0, data)[i].values.push_back(			\
      msg->TOPIC_IN_VALUE(elem)						\
      );								\
    BOOST_PP_SEQ_ELEM(0, data)[i].received = true;			\
    ROS_DEBUG("RECV '%s.%s': %.2f.",					\
	      TOPIC_IN_NAME_STR(elem),					\
      	      TOPIC_IN_VALUE_STR(elem),					\
	      msg->TOPIC_IN_VALUE(elem));				\
  }									\
  /**/

/** 
 * @brief Generates the methods for receiving the defined topics.
 */
#define RECEIVES(topics, topicState, fctPrefix)				\
  BOOST_PP_SEQ_FOR_EACH_I(RECEIVE_TOPIC, (topicState)(fctPrefix), topics) \
  /**/

// ----------------------------------------------------------------
// publishing topics
// ----------------------------------------------------------------

/** 
 * @brief Initializes a publisher for a specific topic to output
 * (fixed message type: OutputEntityStamped).
 */
#define PUBLISH_INIT_TOPIC(r, data, i, elem)			\
  BOOST_PP_SEQ_ELEM(0, data)[i] =				\
    BOOST_PP_SEQ_ELEM(1, data).advertise			\
    <sf_msgs::OutputEntityStamped>				\
    (TOPIC_OUT_NAME_STR(elem),					\
     1);							\
  ROS_INFO("Publishing to '%s'.", TOPIC_OUT_NAME_STR(elem));	\
  /**/

/** 
 * @brief Expands to publisher initializations.
 *
 * When defining TOPICS_OUT as: 
 * \code
 * #define TOPICS_OUT	((state_0_fused, 0))
 * \endcode
 * PUBLISH_INIT(publishers,n) expands to:
 * \code
 * publishers[0] = n.advertise<sf_msgs::OutputEntityStamped>("state_0_fused",1);
 * ROS_INFO("Publishing to '%s'.", "state_0_fused");
 * \endcode
 */
#define PUBLISH_INIT(publishers, handle)				\
  BOOST_PP_SEQ_FOR_EACH_I(PUBLISH_INIT_TOPIC, (publishers)(handle), TOPICS_OUT) \
  /**/

/** 
 * @brief Fills a message with the estimate and publishes the updated
 * message.
 *
 * Four assignments are created where value, variance, jitter_ms and
 * the timestamp of a OutputEntityStamped message are set with the
 * data from the estimate (of type Output). Then this updated message
 * is published.
 *
 * @param r Unnecessary, needed by Boost for repetitions.
 * @param data A sequence containing the elements (in order):
 * publishers, messages, estimates.
 * @param i The counter variable, i.e. the index of the tuple in
 * TOPICS_OUT.
 * @param elem The tuple with index i of the sequence TOPICS_OUT,
 * i.e. name of the topic and index of the state variable to publish.
 *
 * Example expansion (first message is filled and published, the
 * message contains information about the second state variable
 * estimate):
 * \code
 * sampleFused[0].value = out[1].getValue();
 * sampleFused[0].variance = out[1].getVariance();
 * sampleFused[0].jitter_ms = out[1].getJitter();
 * sampleFused[0].header.stamp = ros::Time::now();
 * publishers[0].publish(sampleFused[0]);
 * \endcode
 */
#define PUBLISH_TOPIC(r, data, i, elem)					\
  BOOST_PP_SEQ_ELEM(2, data)[i].value =					\
    BOOST_PP_SEQ_ELEM(1, data)[TOPIC_OUT_INDEX(elem)].getValue();	\
  BOOST_PP_SEQ_ELEM(2, data)[i].variance =				\
    BOOST_PP_SEQ_ELEM(1, data)[TOPIC_OUT_INDEX(elem)].getVariance();	\
  BOOST_PP_SEQ_ELEM(2, data)[i].jitter_ms =				\
    BOOST_PP_SEQ_ELEM(1, data)[TOPIC_OUT_INDEX(elem)].getJitter();	\
  BOOST_PP_SEQ_ELEM(2, data)[i].header.stamp = ros::Time::now();	\
  BOOST_PP_SEQ_ELEM(0, data)[i].publish(BOOST_PP_SEQ_ELEM(2, data)[i]);	\
  /**/

/**
 * @brief Publishes the estimates.
 *
 * The states to publish are specified in the sequence of tuples
 * TOPICS_OUT. The tuple contains two elements. The first element is
 * the name of the topic to publish. The second element is the index
 * of the state variable to publish. 
 *
 * \note Be careful to choose a valid state variable index (0
 * .. state-size-1). The size of the state is specified by the state
 * transition model or is 1 with simple estimation methods (Moving
 * Average and Moving Median).
 */
#define PUBLISH(publishers, estimates, messages)			\
  BOOST_PP_SEQ_FOR_EACH_I(PUBLISH_TOPIC, (publishers)(estimates)(messages), TOPICS_OUT) \
  /**/

// ----------------------------------------------------------------
// abbreviations for ASSIGNING a SEQUENCE to a VECTOR
// ----------------------------------------------------------------
/** 
 * @brief Expands to an assignment (e.g. x[0] = 10). 
 *
 * @param r Unnecessary, needed by Boost for repetitions.
 * @param data The name of the vector which should be assigned to.
 * @param i The counter, the position of the element in the vector
 * \c data which should be assigned to.
 * @param elem The value to assign, i.e. the i'th element of a
 * sequence.
 */
#define CODE_LINE_ASSIGN_ELEMENT_TO_VECTOR(r, data, i, elem)	\
  data[i] = elem;						\
  /**/

/**
 * @brief Expands to assignments of a defined vector (sequence of
 * double values) to an Eigen vector.
 *
 * @param vector The vector to be assigned to, i.e. the array which
 * should be filled with the values.
 * @param values_seq The sequence of values to assign to the vector.
 */
#define CODE_ASSIGN_VALUES_TO_VECTOR(vector, values_seq)		\
  BOOST_PP_SEQ_FOR_EACH_I(CODE_LINE_ASSIGN_ELEMENT_TO_VECTOR, vector, values_seq) 

/** 
 * @brief Creates assignments of formulas given in a sequence to a
 * vector.
 *
 * @param vector The vector to be assigned to, i.e. the array which
 * should be filled with the values.
 * @param formulas_seq The sequence of formulas to assign to the
 * vector.
 */
#define CODE_ASSIGN_FORMULAS_TO_VECTOR(vector, formulas_seq)		\
  BOOST_PP_SEQ_FOR_EACH_I(CODE_LINE_ASSIGN_ELEMENT_TO_VECTOR, vector, formulas_seq) 


// ----------------------------------------------------------------
// abbreviations for ASSIGNING a SEQUENCE-of-SEQUENCES to a MATRIX
// ----------------------------------------------------------------

/** @brief Expands to the element in a sequence-of-sequences (a
 * matrix), specified by row and col. */
#define MATRIX_ELEM(formulas, row, col) \
  BOOST_PP_SEQ_ELEM(col, BOOST_PP_SEQ_ELEM(row, formulas))

// macros for second (inner) loop ---------------------------------
#define PRED_2(r, state)				\
  BOOST_PP_NOT_EQUAL(BOOST_PP_TUPLE_ELEM(6, 2, state),	\
		     BOOST_PP_TUPLE_ELEM(6, 3, state))	\
  /**/

#define OP_2(r, state)						\
  (								\
   BOOST_PP_TUPLE_ELEM(6, 0, state),				\
   BOOST_PP_TUPLE_ELEM(6, 1, state),				\
   BOOST_PP_INC( BOOST_PP_TUPLE_ELEM(6, 2, state) ),		\
   BOOST_PP_TUPLE_ELEM(6, 3, state),				\
   BOOST_PP_TUPLE_ELEM(6, 4, state),				\
   BOOST_PP_TUPLE_ELEM(6, 5, state)				\
  )								\
  /**/

#define MACRO_2(r, state)						\
  BOOST_PP_TUPLE_ELEM(6, 4, state)(BOOST_PP_TUPLE_ELEM(6, 0, state),BOOST_PP_TUPLE_ELEM(6, 2, state)) \
  = MATRIX_ELEM( BOOST_PP_TUPLE_ELEM(6, 5, state),			\
		 BOOST_PP_TUPLE_ELEM(6, 0, state),			\
		 BOOST_PP_TUPLE_ELEM(6, 2, state));			\
  /**/

// macros for first (outer) loop ----------------------------------
#define PRED(r, state)						\
  BOOST_PP_NOT_EQUAL( BOOST_PP_TUPLE_ELEM(6, 0, state),		\
		      BOOST_PP_TUPLE_ELEM(6, 1, state) )	\
  /**/

#define OP(r, state)						\
  (								\
   BOOST_PP_INC( BOOST_PP_TUPLE_ELEM(6, 0, state) ),		\
   BOOST_PP_TUPLE_ELEM(6, 1, state),				\
   BOOST_PP_TUPLE_ELEM(6, 2, state),				\
   BOOST_PP_TUPLE_ELEM(6, 3, state),				\
   BOOST_PP_TUPLE_ELEM(6, 4, state),				\
   BOOST_PP_TUPLE_ELEM(6, 5, state)				\
  )								\
  /**/

#define MACRO(r, state)							\
  BOOST_PP_FOR_ ## r( (BOOST_PP_TUPLE_ELEM(6, 0, state),		\
		       BOOST_PP_TUPLE_ELEM(6, 1, state),		\
		       0,						\
		       BOOST_PP_TUPLE_ELEM(6, 3, state),		\
		       BOOST_PP_TUPLE_ELEM(6, 4, state),		\
		       BOOST_PP_TUPLE_ELEM(6, 5, state)),		\
		      PRED_2, OP_2, MACRO_2 )				\
   /**/

/**
 * @brief Expands to assignments of a defined matrix
 * (sequence-of-sequences) to an Eigen matrix.
 * 
 * Initializes an Eigen matrix with values of a macro (e.g. from the
 * configuration header).
 */
#define CODE_ASSIGN_FORMULAS_TO_MATRIX(matrix, formulas)	\
  BOOST_PP_FOR( (0,						\
		 MATRIX_ROWS(formulas),				\
		 0,						\
		 MATRIX_COLS(formulas),				\
		 matrix,					\
		 formulas),					\
		PRED, OP, MACRO )				\
  /**/

#define CODE_ASSIGN_VALUES_TO_MATRIX(matrix, values)	\
  CODE_ASSIGN_FORMULAS_TO_MATRIX(matrix, values)	\
  /**/

#endif // __CONFIGURATION_POSTCONFIG_H__
