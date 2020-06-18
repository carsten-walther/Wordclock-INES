/**
 * ModeServiceController
 */
export class ModeServiceController {

  /**
   * file
   */
  private file: string = '/assets/data/modes.json';

  /**
   * get
   */
  get = async (): Promise<Mode[]> => {
    return fetch(this.file)
      .then(response => response.json())
      .then(data => {
        return data;
      });
  };
}

export const ModeService = new ModeServiceController();
