#include <stdio.h>

#include <vowpalwabbit/vwdll.h>

int main() {
	VW_IOBUF buf;
	char* modelBuf;
	size_t modelBufLen;

	{
		VW_HANDLE h = VW_InitializeA("--loss_function=logistic --link=logistic --confidence --save_resume");

		for (int i = 0; i < 1e3; i++) {
			{
				VW_EXAMPLE goodExample = VW_ReadExampleA(h, "1 |Feature Good");
				VW_Learn(h, goodExample);
				VW_FinishExample(h, goodExample);
			}

			{
				VW_EXAMPLE badExample = VW_ReadExampleA(h, "-1 |Feature Bad");
				VW_Learn(h, badExample);
				VW_FinishExample(h, badExample);
			}
		}

		{
			VW_EXAMPLE goodExample = VW_ReadExampleA(h, "|Feature Good");
			float prediction = VW_Learn(h, goodExample);
			float confidence = VW_GetConfidence(goodExample);
			printf("predict good = %f, confidence = %f\n", prediction, confidence);
			VW_FinishExample(h, goodExample);
		}
		{
			VW_EXAMPLE goodExample = VW_ReadExampleA(h, "|Feature Bad");
			float prediction = VW_Learn(h, goodExample);
			float confidence = VW_GetConfidence(goodExample);
			printf("predict bad = %f, confidence = %f\n", prediction, confidence);
			VW_FinishExample(h, goodExample);
		}
		{
			VW_EXAMPLE goodExample = VW_ReadExampleA(h, "|Feature Unknown");
			float prediction = VW_Learn(h, goodExample);
			float confidence = VW_GetConfidence(goodExample);
			printf("predict unknown = %f, confidence = %f\n", prediction, confidence);
			VW_FinishExample(h, goodExample);
		}

		VW_CopyModelData(h, &buf, &modelBuf, &modelBufLen);
		VW_Finish(h);
	}

	printf("-------\n");

	{
		VW_HANDLE h = VW_InitializeWithModel("--loss_function=logistic --link=logistic --confidence --save_resume", modelBuf, modelBufLen);

		{
			VW_EXAMPLE goodExample = VW_ReadExampleA(h, "|Feature Good");
			float prediction = VW_Learn(h, goodExample);
			float confidence = VW_GetConfidence(goodExample);
			printf("predict good = %f, confidence = %f\n", prediction, confidence);
			VW_FinishExample(h, goodExample);
		}
		{
			VW_EXAMPLE goodExample = VW_ReadExampleA(h, "|Feature Bad");
			float prediction = VW_Learn(h, goodExample);
			float confidence = VW_GetConfidence(goodExample);
			printf("predict bad = %f, confidence = %f\n", prediction, confidence);
			VW_FinishExample(h, goodExample);
		}
		{
			VW_EXAMPLE goodExample = VW_ReadExampleA(h, "|Feature Unknown");
			float prediction = VW_Learn(h, goodExample);
			float confidence = VW_GetConfidence(goodExample);
			printf("predict unknown = %f, confidence = %f\n", prediction, confidence);
			VW_FinishExample(h, goodExample);
		}

		VW_Finish(h);
	}

	VW_FreeIOBuf(buf);
	return 0;
}
