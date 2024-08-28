#include "../../global.h"
#if CONFIG_INPUT_PROCESS

#include "tInputProcess.h"
#include "tMessageReciever.h"

tInputProcess *tInputProcess::Instance = NULL;

void tInputProcess::service()
{
    tDigialInputEvent event;

    event.ShortClick = 0;
    event.LongClick = 0;
    event.DoubleClick = 0;

  for (int i = 0; i < CONFIG_INPUT_PROCESS_NUM_OF_PINS; i++)
  {
    uint8_t State = mButton[i].Process();
    switch (State)
    {
      case tButton::STATE_CLICK:
        event.ShortClick |= (1 << i);
        break;

      case tButton::STATE_LONGCLICK:
        event.LongClick |= (1 << i);
        break;

      case tButton::STATE_DOUBLECLICK:
        event.DoubleClick |= (1 << i);
        break;
    }
  }

  // do we need to send a frame?
  if ((event.ShortClick) | (event.LongClick) | (event.DoubleClick))
  {
      tMessageReciever::Dispatch(MessageType_DigialInputEvent, 0, &event);
  }
}

uint8_t tButton::Process()
{
  uint8_t State = STATE_NO_CHANGE;
  // check current state
  uint8_t CurrentState = GetCurrentState();

  // set mActiveTime
  if (CurrentState == HIGH)
  {
    if (mActiveTime == 0) mActiveTime = 1;
    else if (mActiveTime < 255) mActiveTime += 1;
  }
  else
  {
    mActiveTime = 0;
  }

  if (mActiveTime == SHORT_CLICK_TICKS) State = STATE_CLICK;
  if (mActiveTime == LONG_CLICK_TICKS)  State = STATE_LONGCLICK;

  // set mLastClickTime
  if (mLastClickTime < 255) mLastClickTime += 1;
  if (State == STATE_CLICK)
  {
      if (mLastClickTime <= DOUBLE_CLICK_TICKS_MAX )
      {
            State = STATE_DOUBLECLICK;
      }
      else
      {
             mLastClickTime = 0;
      }
  }

  return State;
}
#endif // CONFIG_INPUT_PROCESS

#if CONFIG_INPUT_PROCESS_INSTANCE

class tInputProcess_defaut : public tInputProcess
{
public:
    tInputProcess_defaut(Scheduler &manager) : tInputProcess(manager) {}

    virtual void setup()
    {
#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 0
        mButton[0].SetPin(CONFIG_INPUT_PROCESS_PIN0,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY0
                1
#else
                0
#endif
        );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 1
        mButton[1].SetPin(CONFIG_INPUT_PROCESS_PIN1,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY1
                1
#else
                0
#endif
        );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 2
        mButton[2].SetPin(CONFIG_INPUT_PROCESS_PIN2,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY2
                1
#else
                0
#endif
        );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 3
        mButton[3].SetPin(CONFIG_INPUT_PROCESS_PIN3,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY3
                1
#else
                0
#endif
        );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 4
        mButton[4].SetPin(CONFIG_INPUT_PROCESS_PIN4,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY4
                1
#else
                0
#endif
        );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 5
        mButton[5].SetPin(CONFIG_INPUT_PROCESS_PIN5,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY5
                1
#else
                0
#endif
        );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 6
        mButton[6].SetPin(CONFIG_INPUT_PROCESS_PIN6,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY6
    1
#else
    0
#endif
  );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 7
        mButton[7].SetPin(CONFIG_INPUT_PROCESS_PIN7,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY7
    1
#else
    0
#endif
  );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 8
        mButton[8].SetPin(CONFIG_INPUT_PROCESS_PIN8,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY8
    1
#else
    0
#endif
  );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 9
        mButton[9].SetPin(CONFIG_INPUT_PROCESS_PIN9,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY9
    1
#else
    0
#endif
  );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 10
        mButton[10].SetPin(CONFIG_INPUT_PROCESS_PIN10,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY10
    1
#else
    0
#endif
  );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 11
        mButton[11].SetPin(CONFIG_INPUT_PROCESS_PIN11,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY11
    1
#else
    0
#endif
  );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 12
        mButton[12].SetPin(CONFIG_INPUT_PROCESS_PIN12,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY12
    1
#else
    0
#endif
  );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 13
        mButton[13].SetPin(CONFIG_INPUT_PROCESS_PIN13,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY13
    1
#else
    0
#endif
  );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 14
        mButton[14].SetPin(CONFIG_INPUT_PROCESS_PIN14,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY14
    1
#else
    0
#endif
  );
#endif

#if CONFIG_INPUT_PROCESS_NUM_OF_PINS > 15
        mButton[15].SetPin(CONFIG_INPUT_PROCESS_PIN15,
#if CONFIG_INPUT_PROCESS_PIN_POLARITY15
    1
#else
    0
#endif
  );
#endif
    }

};

tInputProcess_defaut InputProcess(sched);

#endif INPUT_PROCESS_INSTANCE
