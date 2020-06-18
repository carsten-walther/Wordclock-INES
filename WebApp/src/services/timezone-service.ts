/**
 * TimezoneServiceController
 */
export class TimezoneServiceController {

  /**
   * file
   */
  private file: string = '/assets/data/timezones.json';

  /**
   * get
   */
  get = async (): Promise<Timezone[]> => {
    return fetch(this.file)
      .then(response => response.json())
      .then(data => {
        return data;
      });
  };
}

export const TimezoneService = new TimezoneServiceController();
